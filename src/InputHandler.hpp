#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include "AppState.hpp"
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
    static InputHandler* instance;
    AppState& appState;
    Terminal terminal;

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

    InputHandler()
        : appState(AppState::getInstance()) {}

public:
    InputHandler& operator=(const InputHandler&) = delete;

    static InputHandler& getInstance() {
        if (instance == nullptr)
            instance = new InputHandler();
        return *instance;
    }

    void handleInput() {
        terminal.nonBlock(
            [&]() {
                while (appState.getState()) {
                    switch (Terminal::getChar()) {
                        case keyMap(Action::Up):
                            appState.decrementFileIndex();
                            break;

                        case keyMap(Action::Down):
                        case keyCode::Tab:
                            appState.incrementFileIndex();
                            break;

                        case keyMap(Action::Enter)://todo: use switch
                            case keyCode::Enter:
                            if (appState.getCurrentEntry().is_directory()) {
                                fs::current_path(
                                    fs::current_path() / FileProperties::getName(appState.getCurrentEntry())
                                );
                                appState.resetFileIndex();
                            } else if (FileProperties::isExecutable(appState.getCurrentEntry())) {
                                // TODO: implement
                            } else if (appState.getCurrentEntry().is_regular_file()) {
                                FileManager::openFile(
                                    fs::current_path() / appState.getCurrentEntry().path().string()
                                );
                            }
                            break;

                        case keyMap(Action::Back):
                            FileManager::changeDirectory(fs::current_path() / "..");
                            appState.resetFileIndex(); // TODO: make it stay at the same index
                            break;

                        case keyMap(Action::Rename):
                            break;

                        case keyMap(Action::Delete):
                            break;

                        case keyMap(Action::Quit):
                        case keyCode::Esc:
                            appState.setState(false);
                            break;

                        default:
                            break;
                    }
                }
            }
        );
    }
};

InputHandler* InputHandler::instance = nullptr;

#endif //INPUTHANDLER_HPP
