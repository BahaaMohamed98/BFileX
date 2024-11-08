#ifndef APP_CPP
#define APP_CPP

#include "App.hpp"
#include "Terminal++.hpp"

App::App()
    : isRunning_(true), entryIndex(0), reverseEntries(false), showHiddenFiles(false),
      showPreview(true), sortType(SortType::Normal),
      uiUpdated(true), entriesUpdated(true) {

    updateEntries();
    updateUI();
}

App& App::getInstance() {
    if (instance == nullptr)
        instance = new App();
    return *instance;
}

[[nodiscard]] bool App::isRunning() const {
    return isRunning_.load();
}

void App::quit() {
    this->isRunning_.store(false);
}

void App::setEntryIndex(const size_t& index) {
    entryIndex.store(index);
    updateUI();
}

[[nodiscard]] size_t App::getEntryIndex() const {
    return entryIndex.load();
}

void App::incrementEntryIndex() {
    entryIndex = (entryIndex + 1) % static_cast<int>(entries.size());
    updateUI();
}

void App::decrementEntryIndex() {
    if (entryIndex == 0)
        entryIndex = static_cast<int>(entries.size()) - 1;
    else
        --entryIndex;
    updateUI();
}

fs::directory_entry& App::getCurrentEntry() {
    return entries[entryIndex];
}

void App::updateEntries() {
    FileManager::setEntries(
        fs::current_path(),
        getEntries(),
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

void App::setHiddenFiles(const bool& showHiddenFiles) {
    this->showHiddenFiles = showHiddenFiles;
}

[[nodiscard]] bool App::shouldShowHiddenEntries() const {
    return showHiddenFiles;
}

void App::setReverseEntries(const bool& reverseEntries) {
    this->reverseEntries = reverseEntries;
    sortEntries();
}

[[nodiscard]] bool App::shouldReverseEntries() const {
    return reverseEntries;
}

void App::setSortType(const SortType& sortType) {
    this->sortType = sortType;
    sortEntries();
}

[[nodiscard]] SortType App::getSortType() const {
    return sortType;
}

void App::changeDirectory(const fs::path& path) {
    // the current path is the previous parent if we go back
    const fs::path previousParent = fs::current_path();

    // Save the index of the current path for the future
    entriesIndices[fs::current_path()] = entryIndex;

    fs::current_path(path); // change directory
    updateEntries();        // get the new entries

    // if entry visited before get it's stored index
    if (const auto it = entriesIndices.find(fs::current_path()); it != entriesIndices.end()) {
        entryIndex = it->second;
    } else if (path.filename() == fs::path("..") and fs::current_path().has_parent_path()) {
        // when going back highlight the parent of the current directory
        entryIndex = [&]() {
            int index{};

            // we search for the previous' parent index in the current directory
            // and set the current entry index to its index
            for (const auto& entry : entries) {
                if (entry.path() == previousParent)
                    return index;
                ++index;
            }
            return 0; // if somehow not found we return 0
        }();
    } else {
        entryIndex = 0; // if not visited start at the begining
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

void App::setShowPreview(const bool& showPreview) {
    updateUI();
    this->showPreview.store(showPreview);
}

bool App::shouldShowPreview() const {
    return showPreview.load();
}

App* App::instance = nullptr;

#endif //APP_CPP
