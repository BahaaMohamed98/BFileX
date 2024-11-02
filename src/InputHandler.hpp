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
    Rename,
    Delete,
    Quit,
};

class InputHandler {
    static InputHandler* instance;
    AppState& appState;
    Terminal terminal;

    static inline std::unordered_map<char, Action> keyMap{
        {'k', Action::Up},
        {'j', Action::Down}, {keyCode::Tab, Action::Down},
        {'l', Action::Enter}, {keyCode::Enter, Action::Enter},
        {'h', Action::Back},
        {'r', Action::Rename},
        {'d', Action::Delete},
        {'q', Action::Quit}, {keyCode::Esc, Action::Quit},
    };

    InputHandler()
        : appState(AppState::getInstance()) {}

    void handleUp() const {
        appState.decrementFileIndex();
    }

    void handleDown() const {
        appState.incrementFileIndex();
    }

    void handleEnter() const {
        switch (FileProperties::determineEntryType(appState.getCurrentEntry())) {
            case EntryType::Directory:
                FileManager::changeDirectory(
                    fs::current_path() / FileProperties::getName(appState.getCurrentEntry())
                );
                appState.resetFileIndex();
                break;
            case EntryType::Executable:
                // TODO: implement
                break;
            case EntryType::RegularFile:
                FileManager::openFile(
                    fs::current_path() / appState.getCurrentEntry().path().string()
                );
                break;
            default:
                break;
        }
    }

    void handleBack() const {
        FileManager::changeDirectory(fs::current_path() / "..");
        appState.resetFileIndex(); // TODO: make it stay at the same index
    }

    // Todo: implement
    void handleRename() const {}
    void handleDelete() const {}

    void handleQuit() const {
        appState.setState(false);
    }

    [[nodiscard]] static Action getAction(const char& input) {
        if (const auto it = keyMap.find(input); it != keyMap.end())
            return it->second;
        return Action::None;
    }

public:
    InputHandler& operator=(const InputHandler&) = delete;

    static InputHandler& getInstance() {
        if (instance == nullptr)
            instance = new InputHandler();
        return *instance;
    }

    static void bindKey(const char& key, const Action& action) {
        keyMap[key] = action;
    }

    void handleInput() {
        terminal.nonBlock(
            [&]() {
                while (appState.getState()) {
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
};

InputHandler* InputHandler::instance = nullptr;

#endif //INPUTHANDLER_HPP
