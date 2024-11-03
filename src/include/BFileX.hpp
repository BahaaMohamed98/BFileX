#ifndef BFILEX_HPP
#define BFILEX_HPP

#include "InputHandler.hpp"
#include "UI.hpp"

class BFileX {
    UI ui;
    Terminal terminal;
    AppState& appState;
    InputHandler& inputHandler;

    [[nodiscard]] bool terminalResized();

public:
    BFileX();
    void run();
};

#endif // BFILEX_HPP
