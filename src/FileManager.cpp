#include "FileManager.hpp"
#include "FileProperties.hpp"

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
                if (showHidden) {
                    const bool firstHidden = FileProperties::isHidden(first);
                    const bool secondHidden = FileProperties::isHidden(second);

                    if (firstHidden != secondHidden)
                        return applyReverse(firstHidden, reverse);
                }

                const bool firstIsDir = first.is_directory();
                const bool secondIsDir = second.is_directory();
                if (firstIsDir != secondIsDir)
                    return applyReverse(firstIsDir, reverse);

                const std::string a = first.path().string(), b = second.path().string();
                return std::lexicographical_compare(
                    a.begin(), a.end(),
                    b.begin(), b.end(),
                    [&reverse](const char& A, const char& B) {
                        return applyReverse(tolower(A) < tolower(B), reverse);
                    }
                );
            } else if (sortType == SortType::Time) {
                return applyReverse(
                    fs::last_write_time(first.path()) > fs::last_write_time(second.path()),
                    reverse
                );
            } else {
                return first < second;
            }
        });
}

void FileManager::openFile(const std::string& filePath) {
    std::string command;
#ifdef _WIN32
    command = "start";
#elif __linux__
    command = "xdg-open";
#elif __APPLE__
    command = "open";
#endif
    std::system((command + " \'" + filePath + "\'").c_str());
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
