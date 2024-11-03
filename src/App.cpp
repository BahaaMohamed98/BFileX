#ifndef APP_CPP
#define APP_CPP

#include "App.hpp"
#include "Terminal++.hpp"

App::App()
    : isRunning_(true), entryIndex(0), reverseEntries(false), showHiddenFiles(false),
      sortType(SortType::Normal), uiUpdated(true) {
    updateEntries();
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
    if (index > entries.size()) {
        Terminal::clearScreen();
        Terminal(Color::Red).println("wowo");
        exit(1);
    }
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
}

[[nodiscard]] bool App::shouldReverseEntries() const {
    return reverseEntries;
}

void App::setSortType(const SortType& sortType) {
    this->sortType = sortType;
}

[[nodiscard]] SortType App::getSortType() const {
    return sortType;
}

void App::changeDirectory(const fs::path& path) {
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

App* App::instance = nullptr;

#endif //APP_CPP
