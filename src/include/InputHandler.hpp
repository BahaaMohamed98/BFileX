#pragma once

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
    ToggleSortBySize,
    ToggleReverseEntries,
    MakeDirectory,
    ToggleHideEntries,
    ToggleHelp,
    Quit,
};

class InputHandler {
    App& app;
    Terminal terminal;

    static inline std::unordered_map<char, Action> keyMap{
        {'k', Action::Up}, {keyCode::ArrowUp, Action::Up},
        {'j', Action::Down}, {keyCode::ArrowDown, Action::Down}, {keyCode::Tab, Action::Down},
        {'l', Action::Enter}, {keyCode::ArrowRight, Action::Enter}, {keyCode::Enter, Action::Enter},
        {'h', Action::Back}, {keyCode::ArrowLeft, Action::Back},
        {'m', Action::MakeDirectory},
        {'r', Action::Rename},
        {'d', Action::Delete},
        {'t', Action::ToggleSortByTime},
        {'s', Action::ToggleSortBySize},
        {'R', Action::ToggleReverseEntries},
        {'H', Action::ToggleHideEntries},
        {'p', Action::TogglePreview},
        {'q', Action::Quit},
    };

    InputHandler();

    void handleUp() const;
    void handleDown() const;
    void handleEnter() const;
    void handleBack() const;
    void handleMakeDirectory() const;
    void handleRename() const;
    void handleDelete() const;
    void handleToggleSortByTime() const;
    void handleToggleSortBySize() const;
    void handleToggleReverseEntries() const;
    void handleToggleHideEntries() const;
    void handleTogglePreview() const;
    void handleQuit() const;

    [[nodiscard]] static Action getAction(const char& input);
    [[nodiscard]] bool confirmAction(const std::string& prompt, const Color::Code& color = Color::Red) const;
    bool readInputString(const std::string& prompt, std::string& inputBuffer, EntryType entryType) const;

public:
    InputHandler& operator=(const InputHandler&) = delete;

    static InputHandler& getInstance();
    void handleInput();
};
