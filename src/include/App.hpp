#ifndef APP_HPP
#define APP_HPP

#include <atomic>
#include <filesystem>
#include <unordered_map>
#include "FileManager.hpp"

namespace fs = std::filesystem;

class App {
    std::vector<fs::directory_entry> entries{};
    std::atomic<bool> isRunning_;

    std::atomic<size_t> entryIndex;
    std::unordered_map<fs::path, size_t> entriesIndices;

    bool reverseEntries;
    bool showHiddenFiles;
    SortType sortType;

    bool uiUpdated;

    static App* instance;

    App();

public:
    static App& getInstance();
    App(const App&) = delete;
    [[nodiscard]] bool isRunning() const;
    void quit();
    [[nodiscard]] size_t getEntryIndex() const;
    void setEntryIndex(const size_t& index);
    void incrementEntryIndex();
    void decrementEntryIndex();
    fs::directory_entry& getCurrentEntry();
    void updateEntries();
    std::vector<fs::directory_entry>& getEntries();
    void setHiddenFiles(const bool& showHiddenFiles);
    [[nodiscard]] bool shouldShowHiddenEntries() const;
    void setReverseEntries(const bool& reverseEntries);
    [[nodiscard]] bool shouldReverseEntries() const;
    void setSortType(const SortType& sortType);
    [[nodiscard]] SortType getSortType() const;
    void changeDirectory(const fs::path& path);
    bool shouldUpdateUI();
    void updateUI();
};

#endif //APP_HPP
