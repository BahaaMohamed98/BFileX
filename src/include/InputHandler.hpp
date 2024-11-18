#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include "App.hpp"
#include "FileProperties.hpp"
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
    MakeDirectory,
    ToggleHideEntries,
    Quit,
};

class InputHandler {
    static InputHandler* instance;
    App& app;
    Terminal terminal;

    static inline std::unordered_map<char, Action> keyMap{
        {'k', Action::Up}, {keyCode::ArrowUp, Action::Up},
        {'j', Action::Down}, {keyCode::ArrowDown, Action::Down}, {keyCode::Tab, Action::Down},
        {'l', Action::Enter}, {keyCode::ArrowRight, Action::Enter}, {keyCode::Enter, Action::Enter},
        {'h', Action::Back}, {keyCode::ArrowLeft, Action::Back},
        {'r', Action::Rename},
        {'d', Action::Delete},
        {'p', Action::TogglePreview},
        {'t', Action::ToggleSortByTime},
        {'m', Action::MakeDirectory},
        {'H', Action::ToggleHideEntries},
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
    void handleMakeDirectory() const;
    void handleToggleHideEntries() const;
    void handleQuit() const;

    [[nodiscard]] static Action getAction(const char& input);
    [[nodiscard]] bool confirmAction(const std::string& prompt, const Color& color = Color::Red) const;
    bool readInputString(const std::string& prompt, std::string& inputBuffer, EntryType entryType) const;

public:
    InputHandler& operator=(const InputHandler&) = delete;

    static InputHandler& getInstance();
    void handleInput();
};

#endif // INPUTHANDLER_HPP
