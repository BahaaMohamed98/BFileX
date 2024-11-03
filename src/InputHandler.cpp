#ifndef INPUTHANDLER_CPP
#define INPUTHANDLER_CPP

#include "InputHandler.hpp"
#include "FileProperties.hpp"

InputHandler::InputHandler()
    : appState(AppState::getInstance()) {}

void InputHandler::handleUp() const {
    appState.decrementEntryIndex();
}

void InputHandler::handleDown() const {
    appState.incrementEntryIndex();
}

void InputHandler::handleEnter() const {
    if (appState.getCurrentEntry().is_directory()) {
        appState.changeDirectory(
            fs::current_path() / FileProperties::getName(appState.getCurrentEntry())
        );
    } else if (appState.getCurrentEntry().is_regular_file()) {
        FileManager::openFile(
            fs::current_path() / appState.getCurrentEntry().path().string()
        );
    }
}

void InputHandler::handleBack() const {
    appState.changeDirectory(fs::current_path() / "..");
}

// Todo: implement
void InputHandler::handleRename() const {}

void InputHandler::handleDelete() const {}

void InputHandler::handleQuit() const {
    appState.quit();
}

[[nodiscard]] Action InputHandler::getAction(const char& input) {
    if (const auto it = keyMap.find(input); it != keyMap.end())
        return it->second;
    return Action::None;
}

InputHandler& InputHandler::getInstance() {
    if (instance == nullptr)
        instance = new InputHandler();
    return *instance;
}

void InputHandler::bindKey(const char& key, const Action& action) {
    keyMap[key] = action;
}

void InputHandler::handleInput() {
    terminal.nonBlock(
        [&]() {
            while (appState.isRunning()) {
                switch (getAction(Terminal::getChar())) {
                    case Action::Up:
                        handleUp();
                        break;
                    case Action::Down:
                        handleDown();
                        break;
                    case Action::Enter:
                        handleEnter();
                        break;
                    case Action::Back:
                        handleBack();
                        break;
                    case Action::Rename:
                        handleRename();
                        break;
                    case Action::Delete:
                        handleDelete();
                        break;
                    case Action::Quit:
                        handleQuit();
                        break;
                    default:
                        break;
                }
            }
        }
    );
}

InputHandler* InputHandler::instance = nullptr;

#endif // INPUTHANDLER_CPP
