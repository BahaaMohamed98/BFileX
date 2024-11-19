#pragma once

#include <atomic>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include "FileManager.hpp"

namespace fs = std::filesystem;

class App {
    std::vector<fs::directory_entry> entries;
    std::atomic<bool> isRunning_;

    std::atomic<size_t> entryIndex;
    std::unordered_map<fs::path, size_t> entriesIndices;

    bool reverseEntries;
    bool showHiddenEntries;
    std::atomic<bool> showPreview;
    SortType sortType;

    std::function<void()> customFooter;

    bool uiUpdated;
    bool entriesUpdated;

    fs::path previousParent;

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
    std::vector<fs::directory_entry>& getEntries();

    void changeDirectory(const fs::path& path);
    void updateEntries();
    void sortEntries();

    void setShowHiddenEntries(const bool& showHiddenFiles);
    [[nodiscard]] bool shouldShowHiddenEntries() const;

    void setReverseEntries(const bool& reverseEntries);
    [[nodiscard]] bool shouldReverseEntries() const;

    void setSortType(const SortType& sortType);
    [[nodiscard]] SortType getSortType() const;

    void setCustomFooter(const std::function<void()>& customFooter);
    void resetFooter();
    const std::function<void()>& getCustomFooter() const;

    void updateUI();
    bool shouldUpdateUI();

    void setShowPreview(const bool& showPreview);
    [[nodiscard]] bool shouldShowPreview() const;
};
