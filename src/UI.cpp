#include "UI.hpp"
#include "FileProperties.hpp"
#include "Terminal++.hpp"

UI::UI() {
    initialize();

    // setting the terminal dimensions
    auto [nWidth, nHeight] = Terminal::size();
    resize(nWidth, nHeight); // adjust the UI layout to the current terminal size
}

UI::~UI() {
    // restoring the terminal state by
    Cursor::show();                   // show the cursor before exiting
    Screen::enableLineWrap();         // enable line wrapping back
    Screen::disableAlternateScreen(); // disable alternate screen buffer and return to main screen
}

void UI::initialize() {
    // setting up the terminal environment
    Screen::enableAlternateScreen(); // uses the alternate screen buffer
    Terminal::setTitle("BFileX");    // set the terminal title to "BFileX"
    Cursor::hide();                  // hide the cursor for a cleaner UI
    Screen::clear();                 // clear the screen on startup
    Screen::disableLineWrap();       // disable line wrapping for more control and better UI

}

void UI::printEntry(const std::filesystem::directory_entry& entry, const bool highlight) const {
    Printer printer;
    printer.setTextStyle(TextStyle::Bold);

    // get the color based on entry type
    const Color::Code color = FileProperties::Mapper::getColor(entry);

    if (highlight) // highlights the currently selected entry by changing the background color
        printer.setTextColor(Color::Black).setBackgroundColor(color);
    else
        printer.setTextColor(color);

    // construct the string to be printed with icon and file name
    std::string entryStr = FileProperties::Mapper::getIcon(entry).representation +
                           FileProperties::MetaData::getName(entry).string();

    // limiting the number of characters printed to terminal width
    if (static_cast<int>(entryStr.size()) >= highlightWidth)
        entryStr = entryStr.substr(0, highlightWidth - 5) + "~";

    // adding a whitespace for padding and using std::setw() to limit the highlight to its width
    printer.print(" ", entryStr, std::setw(highlightWidth - static_cast<int>(entryStr.size())), " ");

    Printer().println();
}

void UI::renderTopBar(const std::string& currentPath) const {
    std::string topBar = "$ " + currentPath; // storing the prompt, the absolute path, and the current entry's name
    // finding the index of the last seprator to change the color of the selected entry
    const auto lastSlashIndex = topBar.find_last_of(std::filesystem::path::preferred_separator);

    // limiting it to the terminal width
    if (topBar.size() > terminalWidth)
        topBar = topBar.substr(0, terminalWidth - 1) + "~";

    // printing the prompt and the path in blue
    Printer(Color::Blue).setTextStyle(TextStyle::Bold).print(
        topBar.substr(0, std::min(lastSlashIndex + 1, topBar.size()))
    );

    // check if we can print the current entry's name
    if (lastSlashIndex < static_cast<int>(topBar.length()))
        Printer().println(topBar.substr(lastSlashIndex + 1));
}

void UI::renderEntries(const std::vector<fs::directory_entry>& entries, const size_t currentIndex, const int startX,
                       const int startY) {
    const size_t totalEntries = entries.size();
    const size_t maxVisibleEntries = terminalHeight - startY; // display height for the entries

    // return if there's nothing to render
    if (maxVisibleEntries < 1) {
        return;
    }

    // Update the starting index for scrolling
    if (currentIndex >= startingIndex + maxVisibleEntries) {
        // if current index more than starting index: make current index the last displayed element
        startingIndex = currentIndex - maxVisibleEntries + 1;
    } else if (currentIndex < startingIndex) {
        // if current index less than starting index: start from the current index
        startingIndex = currentIndex;
    }

    // calculate end index for display
    const size_t endIndex = std::min(startingIndex + maxVisibleEntries, totalEntries);

    // Tracks the row-wise offset for rendering
    int verticalOffset{};

    // Render visible entries
    for (size_t i = startingIndex; i < endIndex; ++i) {
        Cursor::moveTo(startX, startY + verticalOffset++);
        printEntry(entries[i], i == currentIndex);
    }
}

void UI::renderPreview(const fs::directory_entry& entry) {
    // return if it's a binary file
    if (FileProperties::Utilities::isBinary(entry.path().string())) {
        return;
    }

    const EntryType entryType = FileProperties::Types::determineEntryType(entry);
    
    if (entryType == EntryType::RegularFile) {
        const std::string filePath = FileProperties::MetaData::getName(entry).string();

        // renders the preview for selected file
        filePreview.render(filePath);
    } else if (entryType == EntryType::Directory and not FileProperties::Utilities::isDotDot(entry)) {
        App& app = App::getInstance();

        // render preview for the children of the current entry
        renderEntries(app.getCurrentEntryChildren(), app.getCachedIndex(entry.path()), terminalWidth / 2 + 1, 3);
    }
}

void UI::clearPreview() const {
    filePreview.clearPreview();
}

void UI::renderFooter(App& app) const {
    // move to the bottom of the screen to render the footer
    Cursor::moveTo(1, terminalHeight);
    Screen::clear(ClearType::Line);

    // if a custom footer is set run it and return
    if (const auto& footer = app.getCustomFooter(); footer != nullptr)
        return footer();

    // getting the entry's last write time
    const time_t lastWriteTime = FileProperties::MetaData::getLastWriteTime(app.getCurrentEntry().path());

    // Print file type, permissions, and last write time
    Printer().print(
        // print file type
        // `d` for directories, `l` for symlinks, and  `.` for other types
        app.getCurrentEntry().is_directory() ? "d" : app.getCurrentEntry().is_symlink() ? "l" : ".",

        // print entry's permissions
        FileProperties::MetaData::getPermissionsAsString(app.getCurrentEntry()),

        // padding for spacing
        "  ",

        // formatting used for last write time
        // %a   - Abbreviated weekday name (e.g., Mon)
        // %b   - Full month name (e.g., Dec)
        // %e   - Day of the month (1-31)
        // %r   - Localized 12-hour clock time (e.g., 02:30:45 PM)
        // %Y   - Year (e.g., 2024)
        // see std::put_time documentation for more details: https://en.cppreference.com/w/cpp/io/manip/put_time
        std::put_time(std::localtime(&lastWriteTime), "%a %b %e %r %Y") // Print formatted last write time
    );

    // printing the formatted size of the current entry
    Printer().print("  ", FileProperties::MetaData::getSizeAsString(app.getCurrentEntry()));

    // show the current entry index and total entries in the directory
    const std::string directoryNumber =
            " " + std::to_string(app.getCurrentEntryIndex() + 1) +
            "/" + std::to_string(static_cast<int>(app.getEntries().size()));

    // move to the end of the row to print the directory index information
    Cursor::moveTo(terminalWidth - static_cast<int>(directoryNumber.length()) + 1, terminalHeight);
    Printer().print(directoryNumber);
}

void UI::resize(const int nWidth, const int nHeight) {
    // setting the new terminal dimensions
    terminalWidth = nWidth, terminalHeight = nHeight;
    // the entry highlighting width is limited to half of the screen
    highlightWidth = terminalWidth / 2;

    // resizeing the preview with the new dimensions
    filePreview.resize(terminalWidth, terminalHeight);
}

UI& UI::getInstance() {
    static UI ui;
    return ui;
}
