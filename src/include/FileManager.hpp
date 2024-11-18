#pragma once

#include <filesystem>
#include <vector>

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

public:
    static void sortEntries(
        std::vector<fs::directory_entry>& entries,
        const SortType& sortType,
        const bool& showHidden,
        const bool& reverse
    );

    static void openFile(const std::string& filePath);

    static void setEntries(
        const fs::path& rootPath,
        std::vector<fs::directory_entry>& entries,
        const bool& showHidden,
        const SortType& sortType,
        const bool& reverse
    );

    static int getIndex(const fs::path& target, const std::vector<fs::directory_entry>& entries);
};
