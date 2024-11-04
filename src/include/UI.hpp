#ifndef UI_HPP
#define UI_HPP

#include "App.hpp"
#include "Preview.hpp"
#include "Terminal++.hpp"

class UI {
    int tWidth{};
    int tHeight{};
    int selectionWidth{};

    Terminal term;
    Preview preview;

    size_t startingIndex{};

    void printEntry(const std::filesystem::directory_entry& entry, const bool& highlight = false) const;

public:
    UI();
    ~UI();

    void renderTopBar(const std::string& currentPath) const;
    void renderEntries(const std::vector<fs::directory_entry>& entries, const size_t& currentIndex);
    void renderFooter(App& app, const std::function<void()>& customFooter = nullptr) const;
    void renderPreview(const std::string& filePath);
    void resize(const int& nWidth, const int& nHeight);
};

#endif // UI_HPP
