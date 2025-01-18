#pragma once
#include <filesystem>
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
    Header,
    Cpp,
    Compressed,
    Markdown,
    Unknown
};

class FileProperties {
public:
    struct Icon {
        const std::string representation;

        explicit Icon(std::string icon);
        Icon() = default;
    };

    class Types {
        static inline std::unordered_map<std::string, FileType> extensionType{
            {".txt", FileType::Text},
            {".cpp", FileType::Cpp},
            {".h", FileType::Header},
            {".hpp", FileType::Header},
            {".png", FileType::Image},
            {".jpg", FileType::Image},
            {".mp4", FileType::Video},
            {".webm", FileType::Video},
            {".mkv", FileType::Video},
            {".avi", FileType::Video},
            {".pdf", FileType::PDF},
            {".zip", FileType::Compressed},
            {".tar", FileType::Compressed},
            {".gz", FileType::Compressed},
            {".rar", FileType::Compressed},
            {".md", FileType::Markdown},
        };

    public:
        static EntryType determineEntryType(const fs::directory_entry& entry);
        static FileType determineFileType(const fs::path& filePath);
    };

    class Mapper {
        static inline std::unordered_map<EntryType, Color::Code> colorMap{
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
            {FileType::Header, Icon(" ")},
            {FileType::Compressed, Icon(" ")},
            {FileType::Markdown, Icon(" ")},
            {FileType::Unknown, Icon(" ")},
        };

    public:
        static Icon getIcon(const fs::directory_entry& entry);
        static Color::Code getColor(const fs::directory_entry& entry);
        static Color::Code getColor(EntryType entryType);
    };

    class MetaData {
    public:
        static std::string getPermissionsAsString(const fs::directory_entry& entry);
        static std::string getSizeAsString(const fs::directory_entry& entry);
        static fs::path getName(const fs::directory_entry& entry);
        static std::time_t getLastWriteTime(const fs::path& path);
    };

    class Utilities {
    public:
        static bool isHidden(const std::filesystem::directory_entry& entry);
        static bool isExecutable(const std::filesystem::path& path);
        static bool isBinary(const std::string& path);
        static bool isDotDot(const std::filesystem::path& path);
    };
};

// custom hash function for Icon
template<>
struct std::hash<FileProperties::Icon> {
    std::size_t operator()(const FileProperties::Icon& icon) const noexcept;
};
