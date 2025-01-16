#pragma once

#include "InputHandler.hpp"
#include "UI.hpp"

class BFileX {
    UI ui;
    App& app;
    Terminal terminal;

    static void signalHandler(int);
    [[nodiscard]] bool terminalResized();
    void renderUI();

    void startMainLoop();
    BFileX();

public:
    static void run(int argc, char** argv);
};
