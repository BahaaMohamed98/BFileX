#include "BFileX.hpp"

#include <atomic>
#include <csignal>

#include "App.hpp"
#include "CommandLineParser.hpp"
#include "Terminal++.hpp"
#include "UI.hpp"

void BFileX::signalHandler(int) {
    app.quit();
    ui.~UI();

    exit(EXIT_FAILURE);
}

void BFileX::handleResize(int) {
    static std::atomic_bool resizing{false};

    if (not resizing.exchange(true)) {
        auto [width,height] = Terminal::size();
        ui.resize(width, height);
        app.updateUI();

        resizing.store(false);
    }
}

void BFileX::renderUI() {
    Screen::clear();

    ui.renderTopBar((fs::current_path() / app.getCurrentEntry().path()).string());

    // show preview if enabled
    if (app.shouldShowPreview()) {
        ui.renderPreview(app.getCurrentEntry());
    }

    ui.renderEntries(app.getEntries(), app.getCurrentEntryIndex(), 1, 2);
    ui.renderFooter(app);

    Printer::flush();
}

void BFileX::run(const int argc, char** argv) {
    // handling command line options
    CommandLineParser::parse(argc, argv);

    // setup signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGWINCH, handleResize);

    app.setUiUpdateCallBack(renderUI);
    app.updateUI();

    // handling user input
    InputHandler::handleInput();
}

UI& BFileX::ui = UI::getInstance();
App& BFileX::app = App::getInstance();
