#include "App.hpp"
#include "Terminal++.hpp"

App::App()
    : isRunning_(true), entryIndex(0), reverseEntries(false), showHiddenEntries(false),
      showPreview(true), sortType(SortType::Normal), customFooter(nullptr),
      uiUpdated(true), entriesUpdated(true) {
    updateEntries();
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

void App::setEntryIndex(const size_t index) {
    entryIndex.store(index);
    updateUI();
}

[[nodiscard]] size_t App::getEntryIndex() const {
    return entryIndex.load();
}

void App::incrementEntryIndex() {
    setEntryIndex((getEntryIndex() + 1) % static_cast<int>(entries.size()));
}

void App::decrementEntryIndex() {
    if (getEntryIndex() == 0) {
        setEntryIndex(static_cast<int>(entries.size()) - 1);
    } else {
        setEntryIndex(getEntryIndex() - 1);
    }
}

fs::directory_entry& App::getCurrentEntry() {
    return entries[getEntryIndex()];
}

void App::updateEntries() {
    FileManager::setEntries(
        fs::current_path(),
        getEntries(),
        getSearchQuery(),
        shouldShowHiddenEntries(),
        getSortType(),
        shouldReverseEntries()
    );

    // update the index to be the min between the previous index and the largest index
    setEntryIndex(std::min(getEntryIndex(), getEntries().size() - 1));
    updateUI();
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
    updateEntries();
}

void App::resetSearchQuery() {
    searchQuery.clear();
    updateEntries();
}

std::string App::getSearchQuery() const {
    return searchQuery;
}

void App::setCustomFooter(const std::function<void()>& customFooter) {
    this->customFooter = customFooter;
    updateUI();
}

void App::resetFooter() {
    setCustomFooter(nullptr);
}

const std::function<void()>& App::getCustomFooter() const {
    return customFooter;
}

void App::changeDirectory(const fs::path& path) {
    // the current path is the previous parent if we go back
    const fs::path previousParent = fs::current_path();

    // Save the index of the current path for the future
    entriesIndices[fs::current_path()] = getEntryIndex();

    fs::current_path(path); // change directory
    resetSearchQuery();     // reset search query after changing directory
    updateEntries();        // get the new entries

    // if entry visited before get it's stored index
    if (const auto it = entriesIndices.find(fs::current_path()); it != entriesIndices.end()) {
        setEntryIndex(it->second);
    } else if (path.filename() == fs::path("..") and fs::current_path().has_parent_path()) {
        // when going back highlight the parent of the current directory

        // we search for the previous' parent index in the current directory
        // and set the current entry index to its index
        setEntryIndex(FileManager::getIndex(previousParent, entries));
    } else {
        setEntryIndex(0); // if not visited start at the begining
    }

    updateUI(); // updaing the ui after changing the entries
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
        });
    }
}
