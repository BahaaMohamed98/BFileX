#include <algorithm>
#include <atomic>
#include <filesystem>
#include "Terminal++.hpp"
namespace fs = std::filesystem;

std::atomic<int> width;

// opening the file for user on specific OS
void openFile(const std::string& filePath) {
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

fs::path getName(const fs::directory_entry& entry) {
    return entry.path().filename();
}

void getEntries(const fs::path& rootPath, std::vector<fs::directory_entry>& entries, const bool& all = false,
                const bool& sort = true) {
    entries.clear();

    for (const auto& item : fs::directory_iterator(rootPath)) {
        if (all or item.path().filename().string().front() != '.')
            entries.emplace_back(item);
    }

    entries.emplace_back(".."); // add previous directory

    if (sort) {
        // rank directories higher and sort lexicographically
        std::sort(entries.begin(), entries.end(), [](const auto& first, const auto& second) {
            if (first.is_directory() and !second.is_directory())
                return true;
            if (!first.is_directory() and second.is_directory())
                return false;
            return first.path().string() < second.path().string();
        });
    }
}

Color getColor(const fs::directory_entry& entry) {
    if (entry.is_directory())
        return Color::Blue;
    else if (entry.is_regular_file())
        return Color::Green;
    else
        return Color::White;
}

void printEntry(const fs::directory_entry& entry, const bool& highlight = false) {
    const Color color = getColor(entry);
    Terminal t;
    t.setTextColor(color, true);

    if (highlight)
        t.setBackgroundColor(color).setTextColor(Color::Black, true);

    std::string entryStr = getName(entry).string();

    if (static_cast<int>(entryStr.size()) >= width)     // limiting the number of characters
        entryStr = entryStr.substr(0, width - 5) + "~"; // TODO: remove magic number

    t.print(" ", entryStr, std::setw(width - static_cast<int>(entryStr.size())), " ");
    Terminal(Color::Reset).println();
}

std::string getPermissionsStr(const fs::directory_entry& entry) {
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

int main() {
    Terminal ui;
    std::vector<fs::directory_entry> entries;
    std::atomic<int> fileIndex = 0;

    std::atomic<bool> isRunning{true};
    Terminal::clearScreen();
    Terminal::hideCursor();

    // input loop
    ui.nonBlock(
        [&]() {
            while (isRunning) {
                switch (Terminal::getChar()) {

                    // down
                    case 'j':
                    case keyCode::Tab:
                        fileIndex = (fileIndex + 1) % static_cast<int>(entries.size());
                        break;

                    // up
                    case 'k':
                        if (fileIndex == 0)
                            fileIndex = static_cast<int>(entries.size()) - 1;
                        else
                            --fileIndex;
                        break;

                    // back
                    case 'h':
                        fs::current_path(fs::current_path() / "..");
                        fileIndex = 0; // TODO: make it stay at the same index
                        break;

                    // quit
                    case 'q':
                    case keyCode::Esc:
                        isRunning = false;
                        break;

                    // open file or go into directory
                    case keyCode::Enter:
                    case 'l':
                        if (entries[fileIndex].is_directory()) {
                            fs::current_path(fs::current_path() / getName(entries[fileIndex]));
                            fileIndex = 0;
                        } else if (entries[fileIndex].is_regular_file()) {
                            openFile(fs::current_path() / entries[fileIndex].path().string());
                        }
                    default:
                        break;
                }
            }
        }
    );

    // render loop
    while (isRunning) {
        Terminal::clearScreen();
        getEntries(fs::current_path(), entries, false, true);

        auto [tWidth,tHeight] = Terminal::size();
        width = tWidth / 2;

        // top part
        std::string topBar = "$ " + (fs::current_path() / entries[fileIndex].path().filename()).string();
        const auto lastSlashIndex = topBar.find_last_of('/');

        if (topBar.size() > tWidth)
            topBar = topBar.substr(0, tWidth - 1) + "~";

        Terminal().setTextColor(Color::Blue, true).print(topBar.substr(0, std::min(lastSlashIndex + 1, topBar.size())));

        if (lastSlashIndex < static_cast<int>(topBar.size()))
            ui.println(topBar.substr(lastSlashIndex + 1));

        // entries
        for (int i = 0; i < entries.size(); ++i)
            printEntry(entries[i], i == fileIndex);

        // last write time calculation
        auto lastWrite = fs::last_write_time(entries[fileIndex]);
        auto time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            lastWrite - decltype(lastWrite)::clock::now() + std::chrono::system_clock::now()
        );
        std::time_t cftime = std::chrono::system_clock::to_time_t(time);

        // footer
        Terminal::moveTo(1, tHeight);
        ui.print(
            entries[fileIndex].is_directory() ? "d" : ".", // print file type
            getPermissionsStr(entries[fileIndex]),         // print permissions
            "  ",
            std::put_time(std::localtime(&cftime), "%a %b %e %r %Y") // Print formatted time
        );

        Terminal::flush();
        Terminal::sleep(60);
    }

    Terminal::showCursor();
}
