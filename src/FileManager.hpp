#pragma once
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

// Enum class to specify the sorting criteria for entries
enum class SortType {
    None,   // No sorting applied
    Normal, // sort entries lexicographically (by name) (default)
    Time,   // sort entries by last modification time
    Size,   // sort entries by size on disk
};

class FileManager {
    // reverses the given boolean condition based on the reverse flag
    // returns the opposite of the condition if `reverse` is true
    // used when sorting entries in reverse order
    static bool applyReverse(bool condition, bool reverse);

    // case-insensitive lexicographical comparison
    static bool lexicographicalCompare(const std::string& first, const std::string& second);

    // filters entries based on the search query and returns the new vector
    static std::vector<fs::directory_entry> searchEntries(const std::string& searchQuery,
                                                          const std::vector<fs::directory_entry>& entries);

public:
    // populates the entries vector with directory entries from the given path, with sorting options
    // including flags for showing hidden files and reversing the order
    static void setEntries(
        const fs::path& rootPath,
        std::vector<fs::directory_entry>& entries,
        const std::string& searchQuery,
        bool showHidden,
        SortType sortType,
        bool reverse
    );

    // sorts the given entries vector according to the specified sort type
    static void sortEntries(
        std::vector<fs::directory_entry>& entries,
        SortType sortType,
        bool showHidden,
        bool reverse
    );

    // opens a file using the default system command based on the platform
    // should work on Windows, Linux, and macOS using platform-specific commands
    static void openFile(const fs::path& filePath);
    // opens a file using the user's default editor
    // found from the $EDITOR environment variable
    // falls back to using `openFile()`
    // assumes the file is a text file
    static void openFileInEditor(const fs::path& filePath);
    // returns the index of a path in a given vector of entries
    static int getIndex(const fs::path& target, const std::vector<fs::directory_entry>& entries);
};
