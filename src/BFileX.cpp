#include <csignal>
#ifndef BFILEX_CPP
#define BFILEX_CPP

#include "BFileX.hpp"
#include "App.hpp"
#include "FileProperties.hpp"
#include "Terminal++.hpp"

BFileX::BFileX()
    : app(App::getInstance()), inputHandler(InputHandler::getInstance()) {}

void BFileX::signalHandler(int signal) {
    Terminal::showCursor();
    Terminal::disableAlternateScreen();
    exit(0);
}

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

    // show preview if enabled
    if (app.shouldShowPreview() and
        FileProperties::determineEntryType(app.getCurrentEntry()) == EntryType::RegularFile and
        !FileProperties::isBinary(app.getCurrentEntry().path().string()) // only show preview if it's normal text file
    )
        ui.renderPreview(FileProperties::getName(app.getCurrentEntry()).string());

    Terminal::flush();
}

void BFileX::run() {
    // setup signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    inputHandler.handleInput();

    while (app.isRunning()) {
        Terminal::sleep(60);

        if (app.shouldUpdateUI() or terminalResized())
            renderApp();
    }
}

#endif // BFILEX_CPP
