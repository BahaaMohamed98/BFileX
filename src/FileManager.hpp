#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <algorithm>
#include <filesystem>
#include <vector>
#include <sys/stat.h>

#include "FileProperties.hpp"

namespace fs = std::filesystem;

enum class SortType {
    None,
    Normal,
    Time,
};

class FileManager {
    static bool applyReverse(const bool& condition, const bool& reverse) {
        if (reverse)
            return !condition;
        return condition;
    }

    static void sortEntries(
        std::vector<fs::directory_entry>& entries,
        const SortType& sortType,
        const bool& showHidden,
        const bool& reverse
    ) {

        // rank hidden file higher then directories and sort lexicographically
        std::sort(
            entries.begin(), entries.end(),
            [&](const fs::directory_entry& first, const fs::directory_entry& second) { // todo: fix ".." when in reverse
                if (sortType == SortType::Normal) {
                    // rank hidden files (dot-files) the highest
                    if (showHidden) {
                        if (FileProperties::isHidden(first) and !FileProperties::isHidden(second))
                            return applyReverse(true, reverse);
                        if (!FileProperties::isHidden(first) and FileProperties::isHidden(second))
                            return applyReverse(false, reverse);
                    }

                    // rank directories higher
                    if (first.is_directory() and !second.is_directory())
                        return applyReverse(true, reverse);
                    if (!first.is_directory() and second.is_directory())
                        return applyReverse(false, reverse);

                    // lexicographical sort
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

public:
    static void changeDirectory(const fs::path& path) {
        fs::current_path(path);
    }

    // opening the file for user on specific OS
    static void openFile(const std::string& filePath) {
        std::string command;
#ifdef _WIN32
        // For Windows
        command = "start";
#elif __linux__
        // For Linux
        command = "xdg-open";
#elif __APPLE__
        // For macOS
        command = "open";
#endif
        std::system((command + " \'" + filePath + "\'").c_str());
    }

    static void setEntries(
        const fs::path& rootPath,
        std::vector<fs::directory_entry>& entries,
        const bool& showHidden,
        const SortType& sortType,
        const bool& reverse
    ) {
        entries.clear(); // clear previous entries

        entries.emplace_back(".."); // add previous directory at the beginning

        // add entires and filter based on the all paramter
        for (const auto& item : fs::directory_iterator(rootPath)) {
            if (showHidden or !FileProperties::isHidden(item))
                entries.push_back(item);
        }

        if (sortType != SortType::None)
            sortEntries(entries, sortType, showHidden, reverse);
    }
};

#endif //FILEMANAGER_HPP
