#ifndef FILEPROPERTIES_HPP
#define FILEPROPERTIES_HPP

#include <filesystem>
#include <functional>
#include <unordered_map>
#include "Terminal++.hpp"

namespace fs = std::filesystem;

enum class EntryType {
    Directory,
    Executable,
    Symlink,
    RegularFile,
    Unknown,
};

// RegularFiles
enum class FileType {
    Text,
    Image,
    PDF,
    Video,
    Cpp,
    Compressed,
    Markdown,
    Unknown
};

class FileProperties {
public:
    struct Icon {
        const std::string representation;

        explicit Icon(std::string icon) : representation(std::move(icon)) {}

        Icon() : representation(iconMap[EntryType::Unknown].representation) {}
    };

private:
    static inline std::unordered_map<EntryType, Color> colorMap{
        {EntryType::Directory, Color::Blue},
        {EntryType::Executable, Color::Red},
        {EntryType::Symlink, Color::Cyan},
        {EntryType::RegularFile, Color::Green},
        {EntryType::Unknown, Color::White},
    };

    static inline std::unordered_map<EntryType, Icon> iconMap{
        {EntryType::Directory, Icon(" ")},
        {EntryType::Executable, Icon("  ")},
        {EntryType::Symlink, Icon("  ")},
        {EntryType::RegularFile, Icon(" ")},
        {EntryType::Unknown, Icon("❓ ")},
    };

    static inline std::unordered_map<FileType, Icon> extensionIconMap{
        {FileType::Text, Icon(" ")},
        {FileType::Image, Icon(" ")},
        {FileType::PDF, Icon(" ")},
        {FileType::Video, Icon(" ")},
        {FileType::Cpp, Icon(" ")},
        {FileType::Compressed, Icon(" ")},
        {FileType::Markdown, Icon(" ")},
        {FileType::Unknown, Icon(" ")},
    };

    static inline std::unordered_multimap<std::string, FileType> extensionType{
        {".txt", FileType::Text},

        {".png", FileType::Image},
        {".jpg", FileType::Image},

        {".pdf", FileType::PDF},

        {".zip", FileType::Compressed},
        {".tar", FileType::Compressed},
        {".gz", FileType::Compressed},
        {".rar", FileType::Compressed},

        {".md", FileType::Markdown},
    };

private:
    static EntryType determineEntryType(const fs::directory_entry& entry) {
        if (entry.is_directory())
            return EntryType::Directory;
        if (entry.is_symlink())
            return EntryType::Symlink;
        if (isExecutable(entry.path()))
            return EntryType::Executable;
        if (entry.is_regular_file())
            return EntryType::RegularFile;
        return EntryType::Unknown;
    }

    static FileType determineFileType(const fs::path& filePath) {
        if (const auto it = extensionType.find(filePath.extension()); it != extensionType.end())
            return it->second;
        return FileType::Unknown;
    }

public:
    static Icon getIcon(const fs::directory_entry& entry) {
        if (determineEntryType(entry) == EntryType::RegularFile)
            return extensionIconMap[determineFileType(entry.path())];
        return iconMap[determineEntryType(entry)];
    }

    static Color getColor(const fs::directory_entry& entry) {
        return colorMap[determineEntryType(entry)];
    }

    static bool isHidden(const std::filesystem::directory_entry& entry) {
#ifdef _WIN32 // For Windows
        return (GetFileAttributesW(entry.path().wstring().c_str()) & FILE_ATTRIBUTE_HIDDEN) != 0
#else // For Unix-based systems
        return entry.path().filename().string().front() == '.';
#endif
    }

    static bool isExecutable(const std::filesystem::path& path) {
        return (fs::status(path).permissions() & fs::perms::owner_exec) != fs::perms::none;
    }

    static std::string permissionsToString(const fs::directory_entry& entry) {
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

    static std::time_t getLastWriteTime(const fs::path& path) {
        // last write time calculation
        auto lastWrite = fs::last_write_time(path);
        const auto time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            lastWrite - decltype(lastWrite)::clock::now() + std::chrono::system_clock::now()
        );

        return std::chrono::system_clock::to_time_t(time);
    }
};

// custom hash function for Icon
template<>
struct std::hash<FileProperties::Icon> {
    std::size_t operator()(const FileProperties::Icon& icon) const noexcept {
        return std::hash<std::string>()(icon.representation);
    }
};

#endif //FILEPROPERTIES_HPP
