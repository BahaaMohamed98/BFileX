#ifndef BFILEX_CPP
#define BFILEX_CPP

#include "BFileX.hpp"
#include "App.hpp"
#include "Terminal++.hpp"

BFileX::BFileX()
    : app(App::getInstance()), inputHandler(InputHandler::getInstance()) {}

bool BFileX::terminalResized() {
    if (int nWidth, nHeight; terminal.isResized(nWidth, nHeight)) {
        ui.resize(nWidth, nHeight);
        return true;
    }
    return false;
}

void BFileX::renderApp() {
    Terminal::clearScreen();
    ui.renderTopBar((fs::current_path() / app.getCurrentEntry()).string());
    ui.renderEntries(app.getEntries(), app.getEntryIndex());
    ui.renderFooter(app);
    Terminal::flush();
}

void BFileX::run() {
    inputHandler.handleInput();

    while (app.isRunning()) {
        Terminal::sleep(60);

        if (app.shouldUpdateUI() or terminalResized())
            renderApp();
    }
}

#endif // BFILEX_CPP
