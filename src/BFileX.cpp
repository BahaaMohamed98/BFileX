#include "BFileX.hpp"
#include <csignal>
#include "App.hpp"
#include "CommandLineParser.hpp"
#include "FileProperties.hpp"
#include "Terminal++.hpp"

BFileX::BFileX()
    : app(App::getInstance()) {}

void BFileX::signalHandler(const int signal) {
    App::getInstance().quit();

    Cursor::show();
    Screen::disableAlternateScreen();

    exit(EXIT_FAILURE);
}

bool BFileX::terminalResized() {
    if (int nWidth, nHeight; terminal.isResized(nWidth, nHeight)) {
        ui.resize(nWidth, nHeight);
        return true;
    }
    return false;
}

void BFileX::renderUI() {
    Screen::clear();

    ui.renderTopBar((fs::current_path() / app.getCurrentEntry()).string());

    // show preview if enabled
    if (app.shouldShowPreview()) {
        ui.renderPreview(app.getCurrentEntry());
    }

    ui.renderEntries(app.getEntries(), app.getCurrentEntryIndex(), 1, 2);
    ui.renderFooter(app);

    Printer::flush();
}

void BFileX::startMainLoop() {
    // handling user input
    InputHandler::getInstance().handleInput();

    while (app.isRunning()) {
        // a brief delay to lower flickering and cpu usage
        Terminal::sleep(60);

        // render app only when necessary
        if (app.shouldUpdateUI() or terminalResized()) {
            renderUI();
        }
    }
}

void BFileX::run(const int argc, char** argv) {
    // handling command line options
    CommandLineParser::parse(argc, argv);

    // setup signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    BFileX BFileX;
    BFileX.startMainLoop();
}
