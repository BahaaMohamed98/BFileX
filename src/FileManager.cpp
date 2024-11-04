#ifndef FILEMANAGER_CPP
#define FILEMANAGER_CPP

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
            if (sortType == SortType::Normal) {
                if (showHidden) {
                    if (FileProperties::isHidden(first) && !FileProperties::isHidden(second))
                        return applyReverse(true, reverse);
                    if (!FileProperties::isHidden(first) && FileProperties::isHidden(second))
                        return applyReverse(false, reverse);
                }

                if (first.is_directory() && !second.is_directory())
                    return applyReverse(true, reverse);
                if (!first.is_directory() && second.is_directory())
                    return applyReverse(false, reverse);

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

    entries.emplace_back(".."); // add previous directory at the beginning

    for (const auto& item : fs::directory_iterator(rootPath)) {
        if (showHidden || !FileProperties::isHidden(item))
            entries.push_back(item);
    }

    if (sortType != SortType::None)
        sortEntries(entries, sortType, showHidden, reverse);
}

#endif //FILEMANAGER_CPP
