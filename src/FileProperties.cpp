#ifndef FILEPROPERTIES_CPP
#define FILEPROPERTIES_CPP

#include "FileProperties.hpp"

// Constructor implementations
FileProperties::Icon::Icon(std::string icon) : representation(std::move(icon)) {}
FileProperties::Icon::Icon() : representation(iconMap[EntryType::Unknown].representation) {}

// Static member function implementations
EntryType FileProperties::determineEntryType(const fs::directory_entry& entry) {
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

FileType FileProperties::determineFileType(const fs::path& filePath) {
    if (const auto it = extensionType.find(filePath.extension()); it != extensionType.end())
        return it->second;
    return FileType::Unknown;
}

FileProperties::Icon FileProperties::getIcon(const fs::directory_entry& entry) {
    if (determineEntryType(entry) == EntryType::RegularFile)
        return extensionIconMap[determineFileType(entry.path())];
    return iconMap[determineEntryType(entry)];
}

Color FileProperties::getColor(const fs::directory_entry& entry) {
    return colorMap[determineEntryType(entry)];
}

bool FileProperties::isHidden(const std::filesystem::directory_entry& entry) {
#ifdef _WIN32 // For Windows
    return (GetFileAttributesW(entry.path().wstring().c_str()) & FILE_ATTRIBUTE_HIDDEN) != 0;
#else // For Unix-based systems
    return entry.path().filename().string().front() == '.';
#endif
}

bool FileProperties::isExecutable(const std::filesystem::path& path) {
    return (fs::status(path).permissions() & fs::perms::owner_exec) != fs::perms::none;
}

std::string FileProperties::permissionsToString(const fs::directory_entry& entry) {
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

fs::path FileProperties::getName(const fs::directory_entry& entry) {
    return entry.path().filename();
}

std::time_t FileProperties::getLastWriteTime(const fs::path& path) {
    // last write time calculation
    auto lastWrite = fs::last_write_time(path);
    const auto time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        lastWrite - decltype(lastWrite)::clock::now() + std::chrono::system_clock::now()
    );

    return std::chrono::system_clock::to_time_t(time);
}

// Custom hash function for Icon
std::size_t std::hash<FileProperties::Icon>::operator()(const FileProperties::Icon& icon) const noexcept {
    return std::hash<std::string>()(icon.representation);
}

#endif // FILEPROPERTIES_CPP
