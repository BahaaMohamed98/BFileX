#include "FileManager.hpp"
#include <algorithm>
#include <filesystem>
#include "FileProperties.hpp"

bool FileManager::applyReverse(const bool condition, const bool reverse) {
    if (reverse)
        return not condition;
    return condition;
}

bool FileManager::lexicographicalCompare(const std::string& first, const std::string& second) {
    return std::lexicographical_compare(
        first.begin(), first.end(),
        second.begin(), second.end(),
        [](const char a, const char b) {
            // convert characters to lowercase to compare without case sensitivity
            return tolower(a) < tolower(b);
        }
    );
}

std::vector<fs::directory_entry> FileManager::searchEntries(const std::string& searchQuery,
                                                            const std::vector<fs::directory_entry>& entries) {
    std::vector<fs::directory_entry> results;

    for (const auto& entry : entries) {
        std::string name = entry.path().filename().string();

        const bool found = std::search(name.begin(), name.end(), searchQuery.begin(), searchQuery.end(),
                                       [](char a, char b) {
                                           return std::tolower(a) == std::tolower(b);
                                       }) != name.end();

        if (found) {
            results.emplace_back(entry);
        }
    }

    results.emplace_back(fs::path(".."));

    return results;
}

void FileManager::setEntries(
    const fs::path& rootPath,
    std::vector<fs::directory_entry>& entries,
    const std::string& searchQuery,
    const bool showHidden,
    const SortType sortType,
    const bool reverse
) {
    entries.clear(); // clear previous entries

    try {
        entries.emplace_back(".."); // add the previous directory `..` at the top

        for (const auto& item : fs::directory_iterator{rootPath}) {
            // only add the item if it's not hidden or hidden files are allowed
            if (showHidden or not FileProperties::Utilities::isHidden(item)) {
                entries.push_back(item);
            }
        }

        // if there exists a search query
        if (not searchQuery.empty()) {
            // filter out entries based on search results
            entries = searchEntries(searchQuery, entries);
        }

        // sort the entries according to the sort type
        if (sortType != SortType::None) {
            sortEntries(entries, sortType, showHidden, reverse);
        }
    } catch (const fs::filesystem_error&) {
        entries.clear();
        entries.emplace_back("Permission denied!");
    }
}

void FileManager::sortEntries(
    std::vector<fs::directory_entry>& entries,
    const SortType sortType,
    const bool showHidden,
    const bool reverse
) {
    std::sort(
        entries.begin(), entries.end(),
        [&](const fs::directory_entry& first, const fs::directory_entry& second) {
            // rank the previous directory ".." at the top
            if (first.path().filename() == "..") {
                return true;
            }
            if (second.path().filename() == "..") {
                return false;
            }

            if (sortType == SortType::Normal) {
                // rank hidden files higher if shown
                if (showHidden) {
                    const bool firstIsHidden = FileProperties::Utilities::isHidden(first);
                    const bool secondIsHidden = FileProperties::Utilities::isHidden(second);

                    if (firstIsHidden != secondIsHidden) {
                        return applyReverse(firstIsHidden, reverse);
                    }
                }

                // rank directories higher
                const bool firstIsDir = first.is_directory();
                const bool secondIsDir = second.is_directory();

                if (firstIsDir != secondIsDir) {
                    return applyReverse(firstIsDir, reverse);
                }

                // rank based on the lexicogrphical file name comaparision
                return applyReverse(lexicographicalCompare(first.path().string(), second.path().string()), reverse);
            } else if (sortType == SortType::Time) {
                // rank latest modified higher
                return applyReverse(
                    fs::last_write_time(first.path()) > fs::last_write_time(second.path()),
                    reverse
                );
            } else if (sortType == SortType::Size) {
                constexpr int directorySize = 4 * 1024; // used as a default size for directories
                try {
                    const bool firstIsRegularFile = first.is_regular_file();
                    const bool secondIsRegularFile = second.is_regular_file();

                    // compare regurlar files' sizes directly
                    if (firstIsRegularFile and secondIsRegularFile) {
                        return applyReverse(fs::file_size(first.path()) > fs::file_size(second.path()), reverse);
                    } // use `directorySize` as a size for all directories
                    else if (firstIsRegularFile and second.is_directory()) {
                        return applyReverse(fs::file_size(first.path()) > directorySize, reverse);
                    } else if (first.is_directory() and secondIsRegularFile) {
                        return applyReverse(directorySize > fs::file_size(second.path()), reverse);
                    } else { // if they have the same size or are both directories sort lexicographically
                        return applyReverse(
                            lexicographicalCompare(first.path().string(), second.path().string()),
                            reverse
                        );
                    }
                } catch (...) {} // fallback in case of exceptions
                return applyReverse(first.is_regular_file(), reverse);
            } else {
                return lexicographicalCompare(first.path().string(), second.path().string());
            }
        });
}

void FileManager::openFile(const fs::path& filePath) {
    std::string command;
#ifdef __linux__
    command = "xdg-open";
#else
    command = "open";
#endif
    std::system((command + " \'" + filePath.string() + "\'").c_str());
}

void FileManager::openFileInEditor(const fs::path& filePath) {
    static const char* editor = std::getenv("EDITOR");

    if (not editor) {
        openFile(filePath);
        return;
    }

    std::system((std::string{editor} + " \'" + filePath.string() + "\'").c_str());
}

int FileManager::getIndex(const fs::path& target, const std::vector<fs::directory_entry>& entries) {
    int index{};

    // iterate over the entries and find the matching path
    for (const auto& entry : entries) {
        if (entry.path() == target)
            return index; // return the index if found
        ++index;
    }

    //  return 0 if not found
    return 0;
}
