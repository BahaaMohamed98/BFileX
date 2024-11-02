#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include "App.hpp"
#include "FileManager.hpp"
#include "Terminal++.hpp"

enum class Action {
    None,
    Up,
    Down,
    Enter,
    Back,
    Quit,
    Rename,
    Delete
};

class InputHandler {
    App& app;
    Terminal t;

    [[nodiscard]] constexpr static char keyMap(const Action& action) {
        switch (action) {
            case Action::Up:
                return 'k';
            case Action::Down:
                return 'j';
            case Action::Enter:
                return 'l';
            case Action::Back:
                return 'h';
            case Action::Rename:
                return 'r';
            case Action::Delete:
                return 'd';
            case Action::Quit:
                return 'q';
            default:
                return '.';
        }
    }

public:
    InputHandler()
        : app(App::getInstance()) {}

    void handleInput() {
        t.nonBlock(
            [&]() {
                while (app.isRunning) {
                    switch (Terminal::getChar()) {
                        case keyMap(Action::Up):
                            if (app.fileIndex == 0)
                                app.fileIndex = static_cast<int>(app.getEntries().size()) - 1;
                            else
                                --app.fileIndex;
                            break;

                        case keyMap(Action::Down):
                        case keyCode::Tab:
                            if (!app.getEntries().empty())
                                app.fileIndex = (app.fileIndex + 1) % static_cast<int>(app.getEntries().size());

                            break;

                        case keyMap(Action::Enter):
                        case keyCode::Enter:
                            if (app.getEntries()[app.fileIndex].is_directory()) {
                                fs::current_path(
                                    fs::current_path() / FileManager::getName(app.getEntries()[app.fileIndex])
                                );
                                app.fileIndex = 0;
                            } else if (FileManager::isExecutable(app.getEntries()[app.fileIndex])) {
                                // TODO: implement
                            } else if (app.getEntries()[app.fileIndex].is_regular_file()) {
                                FileManager::openFile(
                                    fs::current_path() / app.getEntries()[app.fileIndex].path().string()
                                );
                            }
                            break;

                        case keyMap(Action::Back):
                            FileManager::changeDirectory(fs::current_path() / "..");
                            app.fileIndex = 0; // TODO: make it stay at the same index
                            break;

                        case keyMap(Action::Rename):
                            break;

                        case keyMap(Action::Delete):
                            break;

                        case keyMap(Action::Quit):
                        case keyCode::Esc:
                            app.isRunning = false;
                            break;

                        default:
                            break;
                    }
                }
            }
        );
    }
};

#endif //INPUTHANDLER_HPP
