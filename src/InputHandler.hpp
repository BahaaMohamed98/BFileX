#pragma once

#include "App.hpp"
#include "FileProperties.hpp"
#include "../include/Terminal++/src/Terminal++.hpp"

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
    CreateFile,
    ToggleHideEntries,
    ToggleHelp,
    ToggleSearch,
    SetStartingDirectory,
    ESC,
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
        {'c', Action::CreateFile},
        {'r', Action::Rename},
        {'d', Action::Delete},
        {'t', Action::ToggleSortByTime},
        {'s', Action::ToggleSortBySize},
        {'R', Action::ToggleReverseEntries},
        {'H', Action::ToggleHideEntries},
        {'p', Action::TogglePreview},
        {'/', Action::ToggleSearch},
        {keyCode::Esc, Action::ESC},
        {'q', Action::Quit},
    };

    InputHandler();

    void handleUp() const;
    void handleDown() const;
    void handleEnter() const;
    void handleBack() const;
    void handleMakeDirectory() const;
    void handleCreateFile() const;
    void handleRename() const;
    void handleDelete() const;
    void handleToggleSortByTime() const;
    void handleToggleSortBySize() const;
    void handleToggleReverseEntries() const;
    void handleToggleHideEntries() const;
    void handleTogglePreview() const;
    void handleToggleSearch() const;
    void handleQuit() const;

    [[nodiscard]] static Action getAction(char input);
    [[nodiscard]] bool confirmAction(std::string_view, const Color::Code& color = Color::Red) const;
    bool readInputString(std::string_view prompt, std::string& inputBuffer, EntryType entryType) const;
    void inputLoop() const;

public:
    static void handleInput();
};
