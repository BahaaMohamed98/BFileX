#include "FileProperties.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>

FileProperties::Icon::Icon(std::string icon) : representation(std::move(icon)) {}

EntryType FileProperties::Types::determineEntryType(const fs::directory_entry& entry) {
    if (entry.is_symlink())
        return EntryType::Symlink;
    if (entry.is_directory())
        return EntryType::Directory;
    if (Utilities::isExecutable(entry.path()))
        return EntryType::Executable;
    if (entry.is_regular_file())
        return EntryType::RegularFile;
    return EntryType::Unknown;
}

FileType FileProperties::Types::determineFileType(const fs::path& filePath) {
    if (const auto it = extensionType.find(filePath.extension().string()); it != extensionType.end())
        return it->second;
    return FileType::Unknown;
}

FileProperties::Icon FileProperties::Mapper::getIcon(const fs::directory_entry& entry) {
    const EntryType entryType = Types::determineEntryType(entry);
    if (entryType == EntryType::RegularFile)
        return extensionIconMap[Types::determineFileType(entry.path())];
    return iconMap[entryType];
}

Color::Code FileProperties::Mapper::getColor(const fs::directory_entry& entry) {
    return colorMap[Types::determineEntryType(entry)];
}

Color::Code FileProperties::Mapper::getColor(const EntryType entryType) {
    return colorMap[entryType];
}

std::string FileProperties::MetaData::getPermissionsAsString(const fs::directory_entry& entry) {
    try {
        std::string ret;
        const auto permissions = fs::status(entry.path()).permissions();

        auto appendPermission = [&](const std::string_view ch, const fs::perms& permission) {
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
    } catch (...) {
        return ""; // return an empty string if an exception is raised (due to an api error)
    }
}

fs::path FileProperties::MetaData::getName(const fs::directory_entry& entry) {
    return entry.path().filename();
}

std::time_t FileProperties::MetaData::getLastWriteTime(const fs::path& path) {
    try {
        // last write time calculation
        auto lastWrite = fs::last_write_time(path);
        const auto time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            lastWrite - decltype(lastWrite)::clock::now() + std::chrono::system_clock::now()
        );

        return std::chrono::system_clock::to_time_t(time);
    } catch (...) {
        return -1; // in case of an error return -1
    }
}

std::string FileProperties::MetaData::getSizeAsString(const fs::directory_entry& entry) {
    // return a fixed size string for directories (default on Linux)
    if (entry.is_directory())
        return "4 KB";

    try {
        // get the file size of the entry in bytes (may throw an exception)
        auto fileSize = static_cast<double>(fs::file_size(entry.path()));

        int power{}; // represents the exponent for 1024 (e.g., 1 for KB, 2 for MB, etc.).
        // determine the appropriate size suffix and round the size down?
        for (; fileSize >= 1024.0; ++power) {
            fileSize /= 1024.0;
        }

        std::stringstream ss;

        // don't print the decimal point if the size is an integer
        // else format the size with a precision of 1 digit after the decimal point
        ss << std::fixed << std::setprecision(static_cast<int>(fileSize) == fileSize ? 0 : 1);

        // suffixes for size units (B, KB, MB, etc.)
        constexpr char suffixes[] = "BKMGTPE";

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

bool FileProperties::Utilities::isHidden(const std::filesystem::directory_entry& entry) {
    return entry.path().filename().string().front() == '.';
}

bool FileProperties::Utilities::isExecutable(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary); // Open the file in binary mode

    if (not file.is_open()) {
        return false; // Return false if the file cannot be opened
    }

    // Read the first 4 bytes of the file to cover both ELF (4 bytes) and PE (2 bytes)
    std::vector<unsigned char> buffer(4);
    file.read(reinterpret_cast<char*>(buffer.data()), 4);
    const size_t bytesRead = file.gcount();

    if (bytesRead >= 4) {
        // File is an ELF executable
        if (buffer.size() == 4 and
            buffer[0] == 0x7F and
            buffer[1] == 'E' and
            buffer[2] == 'L' and
            buffer[3] == 'F')
            return true;;
    }

    if (bytesRead >= 2) {
        // File is a PE executable
        return buffer[0] == 'M' and buffer[1] == 'Z';
    }

    return false;
}

bool FileProperties::Utilities::isBinary(const std::string& path) {
    std::ifstream file(path, std::ios::binary);

    if (not file.is_open()) {
        return false;
    }

    // limiting the checking to the first 1 KB
    constexpr size_t bufferSize = 1024;
    char buffer[bufferSize];

    // read the first 1kb into the file
    file.read(buffer, bufferSize);

    // checking if any of ther chracter read is not normal
    return std::any_of(
        buffer, buffer + file.gcount(),
        [](const unsigned char c) {
            return c < 0x20 && c != '\t' && c != '\n' && c != '\r';
        }
    );
}

bool FileProperties::Utilities::isDotDot(const std::filesystem::path& path) {
    return path.filename() == fs::path("..");
}

// Custom hash function for Icon
std::size_t std::hash<FileProperties::Icon>::operator()(const FileProperties::Icon& icon) const noexcept {
    return std::hash<std::string>()(icon.representation);
}
