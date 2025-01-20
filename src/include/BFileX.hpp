#pragma once

#include "App.hpp"
#include "UI.hpp"

class BFileX {
    static UI& ui;
    static App& app;

    static void signalHandler(int);
    static void handleResize(int);
    static void renderUI();

public:
    static void run(int argc, char** argv);
};
