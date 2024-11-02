#ifndef APP_HPP
#define APP_HPP

#include <atomic>
#include <filesystem>
#include <vector>
#include "FileManager.hpp"

namespace fs = std::filesystem;

class AppState {
    std::vector<fs::directory_entry> entries{};
    std::atomic<bool> isRunning;

    std::atomic<int> fileIndex;
    bool reverseEntries;
    bool showHiddenFiles;
    SortType sortType;

    bool indexUpdated; // initilized to true for initial UI render
    fs::path previousPath;

    static AppState* instance;

    AppState()
        : isRunning(true), fileIndex(0), reverseEntries(false), showHiddenFiles(false),
          sortType(SortType::Normal),
          indexUpdated(true), previousPath(fs::current_path()) {}

public:
    static AppState& getInstance() {
        if (instance == nullptr)
            instance = new AppState();
        return *instance;
    }

    AppState(const AppState&) = delete;

    [[nodiscard]] bool getState() const {
        return isRunning.load();
    }

    void setState(const bool& isRunning) {
        this->isRunning.store(isRunning);
    }

    [[nodiscard]] int getFileIndex() const {
        return fileIndex.load();
    }

    void incrementFileIndex() {
        fileIndex = (fileIndex + 1) % static_cast<int>(entries.size());
        indexUpdated = true;
    }

    void decrementFileIndex() {
        if (fileIndex == 0)
            fileIndex = static_cast<int>(entries.size()) - 1;
        else
            --fileIndex;
        indexUpdated = true;
    }

    void resetFileIndex() {
        fileIndex = 0;
        indexUpdated = true;
    }

    fs::directory_entry& getCurrentEntry() {
        return entries[fileIndex];
    }

    std::vector<fs::directory_entry>& getEntries() {
        return entries;
    }

    void setHiddenFiles(const bool& showHiddenFiles) {
        this->showHiddenFiles = showHiddenFiles;
    }

    [[nodiscard]] bool shouldShowHiddenEntries() const {
        return showHiddenFiles;
    }

    [[nodiscard]] bool isPathUpdated() {
        if (const auto currentPath = fs::current_path(); previousPath != currentPath) {
            previousPath = currentPath;
            return true;
        }
        return false;
    }

    [[nodiscard]] bool isIndexUpdated() {
        if (indexUpdated) {
            indexUpdated = false;
            return true;
        }
        return false;
    }

    void setReverseEntries(const bool& reverseEntries) {
        this->reverseEntries = reverseEntries;
    }

    [[nodiscard]] bool shouldReverseEntries() const {
        return reverseEntries;
    }

    void setSortType(const SortType& sortType) {
        this->sortType = sortType;
    }

    [[nodiscard]] SortType getSortType() const {
        return sortType;
    }
};

AppState* AppState::instance = nullptr;

#endif //APP_HPP
