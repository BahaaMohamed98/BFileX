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

        terminalWidth = width;
        terminalHeight = height;

        ui.resize(width, height);
        app.updateUI();

        resizing.store(false);
    }
}

void BFileX::renderUI() {
    // check if a full UI re-render is needed:
    // - The current entry index was changed
    // - Terminal was resized
    // - Entries list was updated

    if (previousIndex != app.getCurrentEntryIndex() or
        std::tie(previousWidth, previousHeight) != std::tie(terminalWidth, terminalHeight) or
        previousEntries != app.getEntries()
    ) { // full UI re-render
        Screen::clear();
        ui.renderTopBar((fs::current_path() / app.getCurrentEntry().path()).string());

        // render preview if enabled
        if (app.shouldShowPreview()) {
            ui.renderPreview(app.getCurrentEntry());
        }

        // render entries and footer
        ui.renderEntries(app.getEntries(), app.getCurrentEntryIndex(), 1, 2);
        ui.renderFooter(app);
    } else if (previousPreviewOn != app.shouldShowPreview()) {
        // preview state changed
        if (app.shouldShowPreview()) {
            // preview enabled
            ui.renderPreview(app.getCurrentEntry());
        } else {
            // clear preview if it was previously shown but is now disabled
            ui.clearPreview();
        }
    } else {
        // only update the footer if no major changes occurred
        ui.renderFooter(app);
    }

    Printer::flush();

    // Update previous state for comparison in the next render cycle
    previousIndex = app.getCurrentEntryIndex();
    previousPreviewOn = app.shouldShowPreview();
    previousEntries = app.getEntries();
    previousWidth = terminalWidth;
    previousHeight = terminalHeight;
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

size_t BFileX::previousIndex{};
bool BFileX::previousPreviewOn{};
std::vector<fs::directory_entry> BFileX::previousEntries{};

int BFileX::terminalWidth{};
int BFileX::terminalHeight{};
int BFileX::previousWidth{};
int BFileX::previousHeight{};
