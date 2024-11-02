#ifndef BFILEX_HPP
#define BFILEX_HPP

#include "InputHandler.hpp"
#include "UI.hpp"

class BFileX {
    UI ui;
    Terminal terminal;
    AppState& appState;
    InputHandler& inputHandler;

    void updateEntries() const {
        FileManager::setEntries(
            fs::current_path(),
            appState.getEntries(),
            appState.shouldShowHiddenEntries(),
            appState.getSortType(),
            appState.shouldReverseEntries()
        );
    }

    [[nodiscard]] bool pathUpdated() const {
        if (appState.isPathUpdated()) {
            updateEntries();
            return true;
        }
        return false;
    }

    [[nodiscard]] bool terminalResized() {
        if (int nWidth, nHeight; terminal.isResized(nWidth, nHeight)) {
            ui.resize(nWidth, nHeight);
            return true;
        }
        return false;
    }

public:
    BFileX()
        : appState(AppState::getInstance()), inputHandler(InputHandler::getInstance()) {}

    void run() {
        updateEntries();
        inputHandler.handleInput();

        while (appState.getState()) {
            Terminal::sleep(60);

            if (appState.isIndexUpdated() or pathUpdated() or terminalResized())
                ui.renderApp();
        }
    }
};

#endif //BFILEX_HPP
