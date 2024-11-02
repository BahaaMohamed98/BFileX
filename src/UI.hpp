#ifndef UI_HPP
#define UI_HPP
#include "AppState.hpp"
#include "FileProperties.hpp"
#include "Terminal++.hpp"

class UI {
    AppState& app;
    int tWidth{};
    int tHeight{};
    int selectionWidth{};
    Terminal term;

    void printEntry(const fs::directory_entry& entry, const bool& highlight = false) const {
        Terminal terminal;
        terminal.setStyle(Style::Bold);

        const Color color = FileProperties::getColor(entry);

        if (highlight)
            terminal.setTextColor(Color::Black).setBackgroundColor(color);
        else
            terminal.setTextColor(color);

        std::string entryStr = FileProperties::getIcon(entry).representation + FileProperties::getName(entry).string();

        if (static_cast<int>(entryStr.size()) >= selectionWidth)     // limiting the number of characters
            entryStr = entryStr.substr(0, selectionWidth - 5) + "~"; // TODO: remove magic number

        terminal.print(" ", entryStr, std::setw(selectionWidth - static_cast<int>(entryStr.size())), " ");

        Terminal(Color::Reset).println();
    }

public:
    UI()
        : app(AppState::getInstance()) {
        Terminal::setTitle("BFilex");
        Terminal::clearScreen();
        Terminal::hideCursor();

        // setting the terminal dimensions
        auto [nWidth,nHeight] = Terminal::size();
        resize(nWidth, nHeight);
    }

    ~UI() {
        Terminal::showCursor();
    }

    void renderTopBar() const {
        std::string topBar = "$ " + (fs::current_path() / app.getCurrentEntry().path().filename()).string();
        const auto lastSlashIndex = topBar.find_last_of(fs::path::preferred_separator);

        if (topBar.size() > tWidth)
            topBar = topBar.substr(0, tWidth - 1) + "~";

        Terminal(Color::Blue)
                .setStyle(Style::Bold).print(
                    topBar.substr(0, std::min(lastSlashIndex + 1, topBar.size()))
                );

        if (lastSlashIndex < static_cast<int>(topBar.length()))
            Terminal().println(topBar.substr(lastSlashIndex + 1));
    }

    void renderEntries() const {
        for (int i = 0; i < app.getEntries().size(); ++i)
            printEntry(app.getEntries()[i], i == app.getFileIndex());
    }

    void renderFooter() const {
        const auto lastWriteTime = FileProperties::getLastWriteTime(app.getCurrentEntry().path());

        Terminal::moveTo(1, Terminal::size().height);
        Terminal().print(
            app.getCurrentEntry().is_directory() ? "d" : ".",           // print file type
            FileProperties::permissionsToString(app.getCurrentEntry()), // print permissions
            "  ",
            std::put_time(std::localtime(&lastWriteTime), "%a %b %e %r %Y") // Print formatted time
        );

        const std::string directoryNumber =
                " " + std::to_string(app.getFileIndex() + 1) +
                "/" + std::to_string(static_cast<int>(app.getEntries().size()));

        Terminal::moveTo(tWidth - static_cast<int>(directoryNumber.length()) + 1, tHeight);
        Terminal().print(directoryNumber);
    }

    void resize(const int& nWidth, const int& nHeight) {
        tWidth = nWidth, tHeight = nHeight;
        selectionWidth = tWidth / 2;
    }

    void renderApp() const {
        Terminal::clearScreen();
        renderTopBar();
        renderEntries();
        renderFooter();
        Terminal::flush();
    }
};

#endif //UI_HPP
