#ifndef APP_CPP
#define APP_CPP

#include "AppState.hpp"
#include "Terminal++.hpp"

AppState::AppState()
    : isRunning_(true), entryIndex(0), reverseEntries(false), showHiddenFiles(false),
      sortType(SortType::Normal),
      uiUpdated(true), defaultFooter(true) {
    updateEntries();
}

AppState& AppState::getInstance() {
    if (instance == nullptr)
        instance = new AppState();
    return *instance;
}

[[nodiscard]] bool AppState::isRunning() const {
    return isRunning_.load();
}

void AppState::quit() {
    this->isRunning_.store(false);
}

void AppState::setEntryIndex(const size_t& index) {
    if (index > entries.size()) {
        Terminal::clearScreen();
        Terminal(Color::Red).println("wowo");
        exit(1);
    }
    entryIndex.store(index);
    updateUI();
}

[[nodiscard]] size_t AppState::getEntryIndex() const {
    return entryIndex.load();
}

void AppState::incrementEntryIndex() {
    entryIndex = (entryIndex + 1) % static_cast<int>(entries.size());
    updateUI();
}

void AppState::decrementEntryIndex() {
    if (entryIndex == 0)
        entryIndex = static_cast<int>(entries.size()) - 1;
    else
        --entryIndex;
    updateUI();
}

fs::directory_entry& AppState::getCurrentEntry() {
    return entries[entryIndex];
}

void AppState::updateEntries() {
    FileManager::setEntries(
        fs::current_path(),
        getEntries(),
        shouldShowHiddenEntries(),
        getSortType(),
        shouldReverseEntries()
    );
    updateUI();
}

std::vector<fs::directory_entry>& AppState::getEntries() {
    return entries;
}

void AppState::setHiddenFiles(const bool& showHiddenFiles) {
    this->showHiddenFiles = showHiddenFiles;
}

[[nodiscard]] bool AppState::shouldShowHiddenEntries() const {
    return showHiddenFiles;
}

void AppState::setReverseEntries(const bool& reverseEntries) {
    this->reverseEntries = reverseEntries;
}

[[nodiscard]] bool AppState::shouldReverseEntries() const {
    return reverseEntries;
}

void AppState::setSortType(const SortType& sortType) {
    this->sortType = sortType;
}

[[nodiscard]] SortType AppState::getSortType() const {
    return sortType;
}

void AppState::changeDirectory(const fs::path& path) {
    // Save the index of the current path for the future
    entriesIndices[fs::current_path()] = entryIndex;

    fs::current_path(path); // change directory
    updateEntries();        // get the new entries

    // if entry visited before get it's stored index
    if (const auto it = entriesIndices.find(fs::current_path()); it != entriesIndices.end())
        entryIndex = it->second;
    else
        entryIndex = 0; // if not visited start at the begining

    updateUI();
}

void AppState::setFooter(const std::function<void()>& func) {
    footerFunction = func;
    defaultFooter = false;
}

void AppState::setDefaultFooter() {
    defaultFooter = true;
}

const std::function<void()>& AppState::getFooter() {
    return footerFunction;
}

bool AppState::useDefaultFooter() const {
    return defaultFooter;
}

bool AppState::shouldUpdateUI() {
    if (uiUpdated) {
        uiUpdated = false;
        return true;
    }
    return false;
}

void AppState::updateUI() {
    uiUpdated = true;
}

AppState* AppState::instance = nullptr;

#endif //APP_CPP
