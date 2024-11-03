#ifndef UI_CPP
#define UI_CPP

#include "UI.hpp"
#include "FileProperties.hpp"

UI::UI()
    : appState(AppState::getInstance()) {
    Terminal::setTitle("BFileX");
    Terminal::clearScreen();
    Terminal::hideCursor();

    // setting the terminal dimensions
    auto [nWidth, nHeight] = Terminal::size();
    resize(nWidth, nHeight);
}

UI::~UI() {
    Terminal::showCursor();
}

void UI::printEntry(const std::filesystem::directory_entry& entry, const bool& highlight) const {
    Terminal terminal;
    terminal.setStyle(Style::Bold);

    const Color color = FileProperties::getColor(entry);

    if (highlight)
        terminal.setTextColor(Color::Black).setBackgroundColor(color);
    else
        terminal.setTextColor(color);

    std::string entryStr = FileProperties::getIcon(entry).representation + FileProperties::getName(entry).string();

    // limiting the number of characters to terminal width
    if (static_cast<int>(entryStr.size()) >= selectionWidth)
        entryStr = entryStr.substr(0, selectionWidth - 5) + "~";

    terminal.print(" ", entryStr, std::setw(selectionWidth - static_cast<int>(entryStr.size())), " ");

    Terminal(Color::Reset).println();
}

void UI::renderTopBar() const {
    std::string topBar = "$ " + (std::filesystem::current_path() / FileProperties::getName(appState.getCurrentEntry())).
                         string();
    const auto lastSlashIndex = topBar.find_last_of(std::filesystem::path::preferred_separator);

    if (topBar.size() > tWidth)
        topBar = topBar.substr(0, tWidth - 1) + "~";

    Terminal(Color::Blue)
            .setStyle(Style::Bold).print(
                topBar.substr(0, std::min(lastSlashIndex + 1, topBar.size()))
            );

    if (lastSlashIndex < static_cast<int>(topBar.length()))
        Terminal().println(topBar.substr(lastSlashIndex + 1));
}

void UI::renderEntries() {
    const size_t currentIndex = appState.getEntryIndex();
    const size_t totalEntries = appState.getEntries().size();
    const size_t maxVisibleEntries = tHeight - 2; // display height for the entries

    // if there's nothing to render: return
    if (maxVisibleEntries <= 0)
        return;

    // Update the starting index for scrolling
    if (currentIndex >= startingIndex + maxVisibleEntries)
        // if current index more than starting index: make current index the last displayed element
        startingIndex = currentIndex - maxVisibleEntries + 1;
    else if (currentIndex < startingIndex)
        // if current index less than starting index: start from the current index
        startingIndex = currentIndex;

    // Calculate end index for display
    const size_t endIndex = std::min(startingIndex + maxVisibleEntries, totalEntries);

    // Render visible entries
    const auto& entries = appState.getEntries();
    for (size_t i = startingIndex; i < endIndex; ++i) {
        printEntry(entries[i], i == currentIndex);
    }
}

void UI::renderFooter() const {
    Terminal::moveTo(1, Terminal::size().height);

    if (!appState.useDefaultFooter()) {
        appState.getFooter()();
        return;
    }

    const auto lastWriteTime = FileProperties::getLastWriteTime(appState.getCurrentEntry().path());

    Terminal().print(
        appState.getCurrentEntry().is_directory() ? "d" : ".",           // print file type
        FileProperties::permissionsToString(appState.getCurrentEntry()), // print permissions
        "  ",
        std::put_time(std::localtime(&lastWriteTime), "%a %b %e %r %Y") // Print formatted time
    );

    const std::string directoryNumber =
            " " + std::to_string(appState.getEntryIndex() + 1) +
            "/" + std::to_string(static_cast<int>(appState.getEntries().size()));

    Terminal::moveTo(tWidth - static_cast<int>(directoryNumber.length()) + 1, tHeight);
    Terminal().print(directoryNumber);
}

void UI::resize(const int& nWidth, const int& nHeight) {
    tWidth = nWidth, tHeight = nHeight;
    selectionWidth = tWidth / 2;
}

void UI::renderApp() {
    Terminal::clearScreen();
    renderTopBar();
    renderEntries();
    renderFooter();
    Terminal::flush();
}

#endif // UI_CPP
