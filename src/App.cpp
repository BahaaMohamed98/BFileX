#include "App.hpp"
#include "FileProperties.hpp"
#include "Terminal++.hpp"

App::App()
    : isRunning_(true), entryIndex(0), reverseEntries(false), showHiddenEntries(false),
      showPreview(true), sortType(SortType::Normal), customFooter(nullptr),
      uiUpdated(true), entriesUpdated(true) {
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
    this->isRunning_.store(false);
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
    entryIndex.store(index);
    updateUI();
}

[[nodiscard]] size_t App::getCurrentEntryIndex() const {
    return entryIndex.load();
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

void App::updateEntries(const bool updateUI_) {
    setEntries(getEntries(), fs::current_path());

    // update the index to be the min between the previous index and the largest index
    setCurrentEntryIndex(std::min(getCurrentEntryIndex(), getEntries().size() - 1));

    if (updateUI_) {
        updateUI();
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

void App::setSearchQuery(const std::string& searchQuery) {
    this->searchQuery = searchQuery;
    updateEntries(true);
}

void App::resetSearchQuery() {
    searchQuery.clear();
    updateEntries(false); // get the new entries
}

const std::string& App::getSearchQuery() const {
    return searchQuery;
}

void App::setCustomFooter(const std::function<void()>& customFooter, const bool updateUI_) {
    this->customFooter = customFooter;
    if (updateUI_) {
        updateUI();
    }
}

void App::resetFooter() {
    setCustomFooter(nullptr, true);
}

const std::function<void()>& App::getCustomFooter() const {
    return customFooter;
}

void App::changeDirectory(const fs::path& path) {
    // the current path is the previous parent if we go back
    const fs::path previousParent = fs::current_path();

    // Save the index of the current path for the future
    entriesIndices[fs::current_path()] = getCurrentEntryIndex();

    fs::current_path(path); // change directory
    resetSearchQuery();     // reset search query after changing directory

    if (FileProperties::Utilities::isDotDot(path) and fs::current_path().has_parent_path()) {
        // when going back highlight the parent of the current directory

        // we search for the previous' parent index in the current directory
        // and set the current entry index to its index
        setCurrentEntryIndex(FileManager::getIndex(previousParent, entries));
    } else {
        // if entry visited before get it's stored index
        setCurrentEntryIndex(getCachedIndex(path));
    }
}

bool App::shouldUpdateUI() {
    if (uiUpdated) {
        uiUpdated = false;
        return true;
    }
    return false;
}

void App::updateUI() {
    uiUpdated = true;
}

void App::setShowPreview(const bool showPreview) {
    this->showPreview.store(showPreview);
    updateUI();
}

bool App::shouldShowPreview() const {
    return showPreview.load();
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
