#include "FileProperties.hpp"
#include <fstream>
#include <cmath>

FileProperties::Icon::Icon(std::string icon) : representation(std::move(icon)) {}
FileProperties::Icon::Icon() : representation(iconMap[EntryType::Unknown].representation) {}

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
    if (const auto it = extensionType.find(filePath.extension().string()); it != extensionType.end())
        return it->second;
    return FileType::Unknown;
}

FileProperties::Icon FileProperties::getIcon(const fs::directory_entry& entry) {
    if (determineEntryType(entry) == EntryType::RegularFile)
        return extensionIconMap[determineFileType(entry.path())];
    return iconMap[determineEntryType(entry)];
}

Color::Code FileProperties::getColor(const fs::directory_entry& entry) {
    return colorMap[determineEntryType(entry)];
}

Color::Code FileProperties::getColor(const EntryType& entryType) {
    return colorMap[entryType];
}

bool FileProperties::isHidden(const std::filesystem::directory_entry& entry) {
#ifdef _WIN32 // For Windows
    return (GetFileAttributesW(entry.path().wstring().c_str()) & FILE_ATTRIBUTE_HIDDEN) != 0;
#else // For Unix-based systems
    return entry.path().filename().string().front() == '.';
#endif
}

bool FileProperties::isExecutable(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary); // Open the file in binary mode

    if (!file.is_open())
        return false; // Return false if the file cannot be opened

    // Check for ELF magic number (Linux executables)
    std::vector<unsigned char> elfBuffer(4);
    file.read(reinterpret_cast<char*>(elfBuffer.data()), 4);

    if (elfBuffer.size() == 4 &&
        elfBuffer[0] == 0x7F &&
        elfBuffer[1] == 'E' &&
        elfBuffer[2] == 'L' &&
        elfBuffer[3] == 'F') {
        return true; // File is an ELF executable
    }

    // Reset the file position to read for PE magic number (Windows executables)
    file.clear();  // Clear any error flags
    file.seekg(0); // Move the file cursor back to the beginning

    std::vector<char> peBuffer(2);
    file.read(peBuffer.data(), 2);

    // Check for PE magic number
    return peBuffer.size() == 2 &&
           peBuffer[0] == 'M' && peBuffer[1] == 'Z'; // File is a PE executable
}

bool FileProperties::isBinary(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    return file.is_open() &&
           std::any_of(
               std::istreambuf_iterator(file),
               std::istreambuf_iterator<char>(),
               [](const unsigned char c) {
                   return c < 0x20 && c != '\t' && c != '\n' && c != '\r';
               }
           );
}

std::string FileProperties::permissionsToString(const fs::directory_entry& entry) {
    std::string ret;

    const auto permissions = fs::status(entry.path()).permissions();
    auto appendPermission = [&](const std::string& ch, const fs::perms& permission) {
        ret.append(
            (permissions & permission) == fs::perms::none ? "-" : ch
        );
    };

    appendPermission("r", fs::perms::owner_read);
    appendPermission("w", fs::perms::owner_write);
    appendPermission("x", fs::perms::owner_exec);
    appendPermission("r", fs::perms::group_read);
    appendPermission("w", fs::perms::group_write);
    appendPermission("x", fs::perms::group_exec);
    appendPermission("r", fs::perms::others_read);
    appendPermission("w", fs::perms::others_write);
    appendPermission("x", fs::perms::others_exec);

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

std::string FileProperties::getSizeString(const fs::directory_entry& entry) {
    // return a fixed size string for directories (default on Linux)
    if (entry.is_directory())
        return "4 KB";

    // suffixes for size units (B, KB, MB, etc.)
    constexpr char suffixes[] = "BKMGTPE";

    try {
        // get the file size of the entry in bytes (may throw an exception)
        auto fileSize = static_cast<double>(fs::file_size(entry.path()));

        int power{}; // represents the exponent for 1024 (e.g., 1 for KB, 2 for MB, etc.).
        // determine the appropriate size suffix and round the size down?
        for (; fileSize >= 1024.0; ++power) {
            fileSize /= 1024.0;
        }

        std::stringstream ss;
        ss << std::fixed;

        if (static_cast<int>(fileSize) == std::ceil(fileSize)) {
            // don't print the decimal point if the size is an integer
            ss << std::setprecision(0);
        } else {
            // format the size with a precision of 1 digit after the decimal point
            ss << std::setprecision(1);
        }

        // adding the size with it's suffix
        ss << fileSize << " " << suffixes[power];

        // append 'B' if the size is larger than bytes
        if (power > 0)
            ss << "B";

        return ss.str();
    } catch (...) {
        // return an empty string if an exception occurs
        return "";
    }
}

// Custom hash function for Icon
std::size_t std::hash<FileProperties::Icon>::operator()(const FileProperties::Icon& icon) const noexcept {
    return std::hash<std::string>()(icon.representation);
}
