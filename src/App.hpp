#ifndef APP_HPP
#define APP_HPP

#include <atomic>
#include <filesystem>
#include <vector>

#include "Terminal++.hpp"
namespace fs = std::filesystem;

class App {
    std::vector<fs::directory_entry> entries{};
    // std::atomic<int> selectionWidth{};
    bool showHiddenFiles{};

    static App* appInstance;
    App() = default;

public:
    std::atomic<bool> isRunning{true};
    std::atomic<int> fileIndex{};

    static App& getInstance() {
        if (appInstance == nullptr)
            appInstance = new App();
        return *appInstance;
    }

    App(const App&) = delete;

    std::vector<fs::directory_entry>& getEntries() {
        return entries;
    }

    // void setSelectionWidth(const int& width) {
    //     selectionWidth.store(width);
    // }

    [[nodiscard]] static int getSelectionWidth() {
        return Terminal::size().width / 2;
    }

    void setHiddenFiles(const bool& show) {
        showHiddenFiles = show;
    }

    [[nodiscard]] bool getShowHiddenFiles() const {
        return showHiddenFiles;
    }
};

App* App::appInstance = nullptr;

#endif //APP_HPP
