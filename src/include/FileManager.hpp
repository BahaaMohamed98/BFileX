#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

enum class SortType {
    None,
    Normal,
    Time,
    Size,
};

class FileManager {
    static bool applyReverse(const bool& condition, const bool& reverse) {
        if (reverse)
            return !condition;
        return condition;
    }

    static bool lexicographicalCompare(const std::string& first, const std::string& second);

public:
    static void setEntries(
        const fs::path& rootPath,
        std::vector<fs::directory_entry>& entries,
        const bool& showHidden,
        const SortType& sortType,
        const bool& reverse
    );

    static void sortEntries(
        std::vector<fs::directory_entry>& entries,
        const SortType& sortType,
        const bool& showHidden,
        const bool& reverse
    );

    static void openFile(const std::string& filePath);
    static int getIndex(const fs::path& target, const std::vector<fs::directory_entry>& entries);
};
