#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include "App.hpp"
#include "Terminal++.hpp"

enum class Action {
    None,
    Up,
    Down,
    Enter,
    Back,
    Rename,
    Delete,
    TogglePreview,
    ToggleSortByTime,
    Quit,
};

class InputHandler {
    static InputHandler* instance;
    App& app;
    Terminal terminal;

    static inline std::unordered_map<char, Action> keyMap{
        {'k', Action::Up},
        {'j', Action::Down}, {keyCode::Tab, Action::Down},
        {'l', Action::Enter}, {keyCode::Enter, Action::Enter},
        {'h', Action::Back},
        {'r', Action::Rename},
        {'d', Action::Delete},
        {'p', Action::TogglePreview},
        {'t', Action::ToggleSortByTime},
        {'q', Action::Quit}, {keyCode::Esc, Action::Quit},
    };

    InputHandler();

    void handleUp() const;
    void handleDown() const;
    void handleEnter() const;
    void handleBack() const;
    void handleRename() const;
    void handleDelete() const;
    void handleTogglePreview() const;
    void handleSortByTime() const;
    void handleQuit() const;

    [[nodiscard]] static Action getAction(const char& input);

public:
    InputHandler& operator=(const InputHandler&) = delete;

    static InputHandler& getInstance();
    static void bindKey(const char& key, const Action& action);
    void handleInput();
};

#endif // INPUTHANDLER_HPP
