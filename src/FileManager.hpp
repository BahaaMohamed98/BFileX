#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <algorithm>
#include <filesystem>
#include <vector>
#include <sys/stat.h>
namespace fs = std::filesystem;

enum class SortType {
    None,
    Normal,
    Time,
};

class FileManager {
    static bool isHidden(const std::filesystem::directory_entry& entry) {
#ifdef _WIN32 // For Windows
        return (GetFileAttributes(entry.path().string().c_str()) & FILE_ATTRIBUTE_HIDDEN) != 0;
#else // For Unix-based systems
        return entry.path().filename().string().front() == '.';
#endif
    }

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
                        if (isHidden(first) and !isHidden(second))
                            return applyReverse(true, reverse);
                        if (!isHidden(first) and isHidden(second))
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

    static void getEntires(
        const fs::path& rootPath,
        std::vector<fs::directory_entry>& entries,
        const bool& reverse,
        const SortType& sortType = SortType::Normal,
        const bool& showHidden = false
    ) {
        entries.clear();

        entries.emplace_back(".."); // add previous directory at the beginning

        // add entires and filter based on the all paramter
        for (const auto& item : fs::directory_iterator(rootPath)) {
            if (showHidden or !isHidden(item))
                entries.push_back(item);
        }

        if (sortType != SortType::None)
            sortEntries(entries, sortType, showHidden, reverse);
    }

    static std::string getPermissionsStr(const fs::directory_entry& entry) {
        std::string ret;

        const auto permissions = fs::status(entry.path()).permissions();
        auto appendPermsission = [&](const std::string& ch, const fs::perms& permission) {
            ret.append(
                (permissions & permission) == fs::perms::none ? "-" : ch
            );
        };

        appendPermsission("r", fs::perms::owner_read);
        appendPermsission("w", fs::perms::owner_write);
        appendPermsission("x", fs::perms::owner_exec);
        appendPermsission("r", fs::perms::group_read);
        appendPermsission("w", fs::perms::group_write);
        appendPermsission("x", fs::perms::group_exec);
        appendPermsission("r", fs::perms::others_read);
        appendPermsission("w", fs::perms::others_write);
        appendPermsission("x", fs::perms::others_exec);

        return ret;
    }

    static fs::path getName(const fs::directory_entry& entry) {
        return entry.path().filename();
    }

    static bool isExecutable(const std::filesystem::path& path) {
        return (fs::status(path).permissions() & fs::perms::owner_exec) != fs::perms::none;
    }

    static std::time_t getLastWriteTime(const fs::path& path) {
        auto& app = App::getInstance();

        // last write time calculation
        auto lastWrite = fs::last_write_time(app.getEntries()[app.fileIndex]);
        const auto time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            lastWrite - decltype(lastWrite)::clock::now() + std::chrono::system_clock::now()
        );

        return std::chrono::system_clock::to_time_t(time);
    }
};

#endif //FILEMANAGER_HPP
