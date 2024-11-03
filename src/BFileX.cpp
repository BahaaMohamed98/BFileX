#ifndef BFILEX_CPP
#define BFILEX_CPP

#include "BFileX.hpp"
#include "AppState.hpp"
#include "Terminal++.hpp"

BFileX::BFileX()
    : appState(AppState::getInstance()), inputHandler(InputHandler::getInstance()) {}

bool BFileX::terminalResized() {
    if (int nWidth, nHeight; terminal.isResized(nWidth, nHeight)) {
        ui.resize(nWidth, nHeight);
        return true;
    }
    return false;
}

void BFileX::run() {
    inputHandler.handleInput();

    while (appState.isRunning()) {
        Terminal::sleep(60);

        if (appState.shouldUpdateUI() || terminalResized())
            ui.renderApp();
    }
}

#endif // BFILEX_CPP
