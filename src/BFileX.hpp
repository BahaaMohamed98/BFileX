#pragma once

#include "App.hpp"
#include "UI.hpp"

class BFileX {
    static UI& ui;
    static App& app;

    static size_t previousIndex;
    static bool previousPreviewOn;
    static std::vector<fs::directory_entry> previousEntries;

    static int terminalWidth;
    static int terminalHeight;
    static int previousWidth;
    static int previousHeight;

    static void signalHandler(int);
    static void handleResize(int);
    static void renderUI();
    static void fullRenderUI();

public:
    static void run(int argc, char** argv);
};
