#ifndef BFILEX_HPP
#define BFILEX_HPP

#include "InputHandler.hpp"
#include "UI.hpp"

class BFileX {
    UI ui;
    App& app;
    Terminal terminal;
    InputHandler& inputHandler;

    [[nodiscard]] bool terminalResized();
    void renderApp();

public:
    BFileX();
    void run();
};

#endif // BFILEX_HPP
