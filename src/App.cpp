#include "App.hpp"

#include <atomic>
#include <filesystem>

#include "FileProperties.hpp"
#include "Terminal++.hpp"

App::App()
    : isRunning_(true), entryIndex(0), reverseEntries(false), showHiddenEntries(false),
      showPreview(true), sortType(SortType::Normal), customFooter(nullptr), uiUpdateCallBack(nullptr),
      initializeTerminalCallBack(nullptr) {
    updateEntries(false);
}

App& App::getInstance() {
    static App app;
    return app;
}

[[nodiscard]] bool App::isRunning() const {
    return isRunning_.load();
}

void App::quit() {
    isRunning_.store(false);
}

size_t App::getCachedIndex(const fs::path& entry) const {
    if (const auto it = entriesIndices.find(entry); it != entriesIndices.end()) {
        return it->second;
    }
    return 0;
}

std::vector<fs::directory_entry> App::getCurrentEntryChildren() {
    if (getCurrentEntry().is_directory()) {
        std::vector<fs::directory_entry> children;
        setEntries(children, getCurrentEntry().path());
        return children;
    }

    return {};
}

void App::setCurrentEntryIndex(const size_t index) {
    // update the index to be the min between the previous index and the largest index
    entryIndex = std::min(index, getEntries().size() - 1);
    updateUI();
}

[[nodiscard]] size_t App::getCurrentEntryIndex() const {
    return entryIndex;
}

void App::incrementCurrentEntryIndex() {
    setCurrentEntryIndex((getCurrentEntryIndex() + 1) % static_cast<int>(entries.size()));
}

void App::decrementCurrentEntryIndex() {
    if (getCurrentEntryIndex() == 0) {
        setCurrentEntryIndex(static_cast<int>(entries.size()) - 1);
    } else {
        setCurrentEntryIndex(getCurrentEntryIndex() - 1);
    }
}

fs::directory_entry& App::getCurrentEntry() {
    return entries[getCurrentEntryIndex()];
}

void App::updateEntries(const bool updateIndex) {
    setEntries(getEntries(), fs::current_path());

    if (updateIndex) {
        // make sure the current index is valid
        setCurrentEntryIndex(getCurrentEntryIndex());
    }
}

void App::setEntries(std::vector<fs::directory_entry>& entries, const fs::path& path) const {
    FileManager::setEntries(
        path,
        entries,
        getSearchQuery(),
        shouldShowHiddenEntries(),
        getSortType(),
        shouldReverseEntries()
    );
}

void App::sortEntries() {
    FileManager::sortEntries(
        getEntries(),
        getSortType(),
        shouldShowHiddenEntries(),
        shouldReverseEntries()
    );
    updateUI();
}

std::vector<fs::directory_entry>& App::getEntries() {
    return entries;
}

void App::setShowHiddenEntries(const bool showHiddenFiles) {
    this->showHiddenEntries = showHiddenFiles;
}

[[nodiscard]] bool App::shouldShowHiddenEntries() const {
    return showHiddenEntries;
}

void App::setReverseEntries(const bool reverseEntries) {
    this->reverseEntries = reverseEntries;
    sortEntries();
}

[[nodiscard]] bool App::shouldReverseEntries() const {
    return reverseEntries;
}

void App::setSortType(const SortType sortType) {
    this->sortType = sortType;
    sortEntries();
}

[[nodiscard]] SortType App::getSortType() const {
    return sortType;
}

void App::setSearchQuery(std::string searchQuery) {
    this->searchQuery = std::move(searchQuery);
    updateEntries(false);

    // set the index to first result if there are more than one result
    // else set it to zero ( the `..` entry)
    setCurrentEntryIndex(entries.size() > 1 ? 1 : 0);
}

bool App::resetSearchQuery() {
    if (not searchQuery.empty()) {
        searchQuery.clear();
        return true;
    }
    return false;
}

const std::string& App::getSearchQuery() const {
    return searchQuery;
}

void App::setCustomFooter(std::function<void()> customFooter, const bool updateUI_) {
    this->customFooter = std::move(customFooter);

    if (updateUI_) {
        updateUI();
    }
}

void App::resetFooter(const bool updateUI_) {
    setCustomFooter(nullptr, updateUI_);
}

void App::initializeTerminal() const {
    if (initializeTerminalCallBack != nullptr) {
        initializeTerminalCallBack();
    }
}

void App::setInitalizeTerminalCallBack(std::function<void()> function) {
    initializeTerminalCallBack = std::move(function);
}

const std::function<void()>& App::getCustomFooter() const {
    return customFooter;
}

void App::changeDirectory(const fs::path& path) {
    const fs::path currentPath = fs::current_path();

    // return if trying to go back from root directory
    if (FileProperties::Utilities::isDotDot(path) and currentPath == fs::path("/")) {
        return;
    }

    // the current path is the previous parent if we go back
    const fs::path& previousParent = currentPath;

    // cache the index of the selected entry in the current path
    entriesIndices[currentPath] = getCurrentEntryIndex();

    try {
        fs::current_path(path); // change directory to the given path
        resetSearchQuery();     // reset search query after changing directory
        updateEntries(false);   // get the new entries

        if (FileProperties::Utilities::isDotDot(path) and currentPath.has_parent_path()) {
            // when going back highlight the parent of the current directory

            // search for the previous parent's index in the current directory
            // and set the current entry's index to its index
            setCurrentEntryIndex(FileManager::getIndex(previousParent, entries));
        } else {
            // if entry visited before get it's stored index
            setCurrentEntryIndex(getCachedIndex(path));
        }
    } catch (const fs::filesystem_error&) {
        setCustomFooter([] {
            Printer(Color::Red).setTextStyle(TextStyle::Bold).print("Cannot change directory: Permission denied");
        }, true);
    }
}

void App::updateUI() const {
    static std::atomic_bool updatingUI{false};

    if (not updatingUI.exchange(true)) {
        if (uiUpdateCallBack != nullptr) {
            uiUpdateCallBack();
        }
        updatingUI.store(false);
    }
}

void App::setUiUpdateCallBack(std::function<void()> function) {
    uiUpdateCallBack = std::move(function);
}

void App::setShowPreview(const bool showPreview) {
    this->showPreview = showPreview;
    updateUI();
}

bool App::shouldShowPreview() const {
    return showPreview;
}

void App::setStartingEntry(const fs::path& path) {
    try {
        changeDirectory(path);
    } catch (const fs::filesystem_error&) {
        setCustomFooter([=] {
            Printer(Color::Red).print("Invalid directory path: ", path);
        }, false);
    }
}
