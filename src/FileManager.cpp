#include "FileManager.hpp"
#include "FileProperties.hpp"

bool FileManager::lexicographicalCompare(const std::string& first, const std::string& second) {
    return std::lexicographical_compare(
        first.begin(), first.end(),
        second.begin(), second.end(),
        [](const char a, const char b) {
            return tolower(a) < tolower(b);
        }
    );
}

void FileManager::setEntries(
    const fs::path& rootPath,
    std::vector<fs::directory_entry>& entries,
    const bool& showHidden,
    const SortType& sortType,
    const bool& reverse
) {
    entries.clear(); // clear previous entries

    entries.emplace_back(".."); // add the previous directory `..`

    for (const auto& item : fs::directory_iterator(rootPath)) {
        // if hidden files are allowed or file is not hidden add to the entries
        if (showHidden or !FileProperties::isHidden(item))
            entries.push_back(item);
    }

    // sort the entries according to the sort type
    if (sortType != SortType::None)
        sortEntries(entries, sortType, showHidden, reverse);
}

void FileManager::sortEntries(
    std::vector<fs::directory_entry>& entries,
    const SortType& sortType,
    const bool& showHidden,
    const bool& reverse
) {
    std::sort(
        entries.begin(), entries.end(),
        [&](const fs::directory_entry& first, const fs::directory_entry& second) {
            // rank the previous directory ".." at the top
            if (first.path().filename() == "..")
                return true;
            if (second.path().filename() == "..")
                return false;

            if (sortType == SortType::Normal) {
                // rank hidden files higher if shown
                if (showHidden) {
                    const bool firstIsHidden = FileProperties::isHidden(first);
                    const bool secondIsHidden = FileProperties::isHidden(second);

                    if (firstIsHidden != secondIsHidden)
                        return applyReverse(firstIsHidden, reverse);
                }

                // rank directories higher
                const bool firstIsDir = first.is_directory();
                const bool secondIsDir = second.is_directory();

                if (firstIsDir != secondIsDir)
                    return applyReverse(firstIsDir, reverse);

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
                } catch (...) {}
                return applyReverse(first.is_regular_file(), reverse);
            } else {
                return lexicographicalCompare(first.path().string(), second.path().string());
            }
        });
}

void FileManager::openFile(const std::string& filePath) {
    std::string command;
#ifdef _WIN32
    command = "start \"\"";
#elif __linux__
    command = "xdg-open";
#elif __APPLE__
    command = "open";
#endif
    std::system((command + " \'" + filePath + "\'").c_str());
}

// returns the index of a path in a given vector of entries
int FileManager::getIndex(const fs::path& target, const std::vector<fs::directory_entry>& entries) {
    int index{};

    for (const auto& entry : entries) {
        if (entry.path() == target)
            return index;
        ++index;
    }

    //  return 0 if not found
    return 0;
}
