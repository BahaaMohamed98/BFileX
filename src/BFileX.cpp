#include "BFileX.hpp"
#include <csignal>
#include "App.hpp"
#include "FileProperties.hpp"
#include "Terminal++.hpp"

BFileX::BFileX()
    : app(App::getInstance()), inputHandler(InputHandler::getInstance()) {}

void BFileX::signalHandler(int signal) {
    Cursor::show();
    Screen::disableAlternateScreen();
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
    Screen::clear();

    ui.renderTopBar((fs::current_path() / app.getCurrentEntry()).string());
    ui.renderEntries(app.getEntries(), app.getEntryIndex());
    ui.renderFooter(app);

    // show preview if enabled
    if (app.shouldShowPreview() and
        FileProperties::Types::determineEntryType(app.getCurrentEntry()) == EntryType::RegularFile and
        !FileProperties::Utilities::isBinary(app.getCurrentEntry().path().string()) // only show preview if it's normal text file
    )
        ui.renderPreview(FileProperties::MetaData::getName(app.getCurrentEntry()).string());

    Printer::flush();
}

void BFileX::run() {
    // setup signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // handling user input
    inputHandler.handleInput();

    while (app.isRunning()) {
        // a brief delay to lower flickering and cpu usage
        Terminal::sleep(60);

        // render app only when necessary
        if (app.shouldUpdateUI() or terminalResized())
            renderApp();
    }
}
