#pragma once

#include "App.hpp"
#include "FilePreview.hpp"

class UI {
    // terminal dimensions
    int terminalWidth{};
    int terminalHeight{};

    // width of the highlighting used to indicate the selected entry
    int highlightWidth{};

    // the starting index for rendering directory entries
    size_t startingIndex{};

    // preview renderer for files
    FilePreview filePreview;

    // print a single directory entry with optional highlighting
    void printEntry(const std::filesystem::directory_entry& entry, bool highlight = false) const;

    UI(); // initialize the terminal
public:
    UI(UI&) = delete; // deleting the copy constructor
    ~UI();            // restore the terminal to its previous state

    // render the top bar with the current path
    void renderTopBar(const std::string& currentPath) const;
    // render the entries
    void renderEntries(const std::vector<fs::directory_entry>& entries, size_t currentIndex, int, int);
    // render the footer with file details or a custom set footer
    void renderFooter(App& app) const;
    // render file preview
    void renderPreview(const fs::directory_entry& entry);
    // resize the UI for the terminal
    void resize(int nWidth, int nHeight);

    static UI& getInstance();
};
