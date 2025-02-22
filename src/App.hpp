#pragma once

#include <atomic>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include "FileManager.hpp"

namespace fs = std::filesystem;

class App {
    std::vector<fs::directory_entry> entries;
    std::atomic_bool isRunning_;

    size_t entryIndex;
    std::unordered_map<fs::path, size_t> entriesIndices;

    bool reverseEntries;
    bool showHiddenEntries;
    bool showPreview;
    SortType sortType;

    std::string searchQuery;

    std::function<void()> customFooter;
    std::function<void()> uiUpdateCallBack;
    std::function<void()> initializeTerminalCallBack;

    App();

public:
    static App& getInstance();
    App(const App&) = delete;

    [[nodiscard]] bool isRunning() const;
    void quit();

    [[nodiscard]] size_t getCachedIndex(const fs::path& entry) const;
    [[nodiscard]] std::vector<fs::directory_entry> getCurrentEntryChildren();
    [[nodiscard]] size_t getCurrentEntryIndex() const;

    void setCurrentEntryIndex(size_t index);
    void incrementCurrentEntryIndex();
    void decrementCurrentEntryIndex();

    fs::directory_entry& getCurrentEntry();
    std::vector<fs::directory_entry>& getEntries();

    void changeDirectory(const fs::path& path);
    void updateEntries(bool updateIndex);
    void setEntries(std::vector<fs::directory_entry>& entries, const fs::path& path) const;
    void sortEntries();

    void setShowHiddenEntries(bool showHiddenFiles);
    [[nodiscard]] bool shouldShowHiddenEntries() const;

    void setReverseEntries(bool reverseEntries);
    [[nodiscard]] bool shouldReverseEntries() const;

    void setSortType(SortType sortType);
    [[nodiscard]] SortType getSortType() const;

    void setSearchQuery(std::string searchQuery);
    bool resetSearchQuery();
    const std::string& getSearchQuery() const;

    void setCustomFooter(std::function<void()> customFooter, bool updateUI_);
    const std::function<void()>& getCustomFooter() const;
    void resetFooter(bool updateUI_ = true);

    void initializeTerminal() const;
    void setInitalizeTerminalCallBack(std::function<void()> function);

    void updateUI() const;
    void setUiUpdateCallBack(std::function<void()> function);

    void setShowPreview(bool showPreview);
    [[nodiscard]] bool shouldShowPreview() const;

    void setStartingEntry(const fs::path& path);
};
