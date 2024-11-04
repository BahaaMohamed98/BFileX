#ifndef INPUTHANDLER_CPP
#define INPUTHANDLER_CPP

#include "InputHandler.hpp"
#include "FileProperties.hpp"

InputHandler::InputHandler()
    : app(App::getInstance()) {}

void InputHandler::handleUp() const {
    app.decrementEntryIndex();
}

void InputHandler::handleDown() const {
    app.incrementEntryIndex();
}

void InputHandler::handleEnter() const {
    if (app.getCurrentEntry().is_directory()) {
        app.changeDirectory(
            fs::current_path() / FileProperties::getName(app.getCurrentEntry())
        );
    } else if (app.getCurrentEntry().is_regular_file()) {
        FileManager::openFile(
            fs::current_path() / app.getCurrentEntry().path().string()
        );
    }
}

void InputHandler::handleBack() const {
    app.changeDirectory(fs::current_path() / "..");
}

// Todo: implement
void InputHandler::handleRename() const {}

void InputHandler::handleDelete() const {}

void InputHandler::handleTogglePreview() const {
    app.setShowPreview(!app.shouldShowPreview());
}

void InputHandler::handleSortByTime() const {
    if (app.getSortType() != SortType::Time)
        app.setSortType(SortType::Time);
    else
        app.setSortType(SortType::Normal);
}

void InputHandler::handleQuit() const {
    app.quit();
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
            while (app.isRunning()) {
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
                    case Action::TogglePreview:
                        handleTogglePreview();
                        break;
                    case Action::ToggleSortByTime:
                        handleSortByTime();
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
