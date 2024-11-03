#ifndef UI_HPP
#define UI_HPP

#include "AppState.hpp"
#include "Terminal++.hpp"

class UI {
    AppState& appState;
    int tWidth{};
    int tHeight{};
    int selectionWidth{};
    Terminal term;

    size_t startingIndex{};

    void printEntry(const std::filesystem::directory_entry& entry, const bool& highlight = false) const;

public:
    UI();
    ~UI();

    void renderTopBar() const;
    void renderEntries();
    void renderFooter() const;
    void resize(const int& nWidth, const int& nHeight);
    void renderApp();
};

#endif // UI_HPP
