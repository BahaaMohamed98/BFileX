#ifndef BFILEX_HPP
#define BFILEX_HPP

#include "InputHandler.hpp"
#include "UI.hpp"

class BFileX {
    InputHandler inputHandler;
    Terminal t;
    App& app;
    UI ui;

public:
    BFileX(): app(App::getInstance()) {}

    void run() {
        inputHandler.handleInput();
        while (app.isRunning) {
            FileManager::getEntires(fs::current_path(), app.getEntries(), false , SortType::Normal, false);
            ui.renderApp();
            // auto [width,height] = Terminal::size();
            // ui.setDimensions(width, height);
            Terminal::sleep(60);
        }
    }
};

#endif //BFILEX_HPP
