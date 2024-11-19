#include "InputHandler.hpp"

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
            (fs::current_path() / app.getCurrentEntry().path()).string()
        );
    }
}

void InputHandler::handleBack() const {
    app.changeDirectory(fs::current_path() / "..");
}

void InputHandler::handleRename() const {
    // initializing the input buffer to the current file name
    std::string inputBuffer = FileProperties::getName(app.getCurrentEntry()).string();

    // return directly if the user tries to rename the `..` directory (do nothing)
    if (inputBuffer == "..")
        return;

    // return if user cancelled
    if (!readInputString("New name: ", inputBuffer, FileProperties::determineEntryType(app.getCurrentEntry())))
        return;

    try {
        // file entry before renaming
        const fs::directory_entry oldEntry = app.getCurrentEntry();
        const fs::path newPath = fs::current_path() / fs::path(inputBuffer);

        // return if the name was not changed
        if (oldEntry.path() == newPath)
            return app.resetFooter();

        // if entry already exists prompt the user about overriding it
        if (fs::exists(newPath)) {
            // return if the answer is not yes
            if (!confirmAction("Entry already exists override it? (y/n) "))
                return app.resetFooter();
        }

        // rename the entry to the specified name
        fs::rename(oldEntry.path(), newPath);

        // refresh the entries stored
        app.updateEntries();

        // set the cursor to point at the same entry after renaming
        app.setEntryIndex(FileManager::getIndex(newPath, app.getEntries()));

        app.setCustomFooter([=] {
            Terminal(Color::Green)
                    .setStyle(Style::Bold)
                    .print("Renamed ", FileProperties::getName(oldEntry), " to: ", newPath.filename());
        });
    } catch (...) {
        app.setCustomFooter([] {
            Terminal(Color::Red).setStyle(Style::Bold).print("Failed to rename entry!");
        });
    }
}

void InputHandler::handleDelete() const {
    const fs::path targetEntry = FileProperties::getName(app.getCurrentEntry());

    // return when trying to delete `..`
    if (targetEntry.string() == "..")
        return;

    // return if the answer is not yes
    if (!confirmAction("Are you sure you want to delete \"" + std::string{targetEntry} + "\"? (y/n) "))
        return app.resetFooter();

    try {
        // if entry is a non empty directory prompt the user about recursively deleting it
        if (FileProperties::determineEntryType(app.getCurrentEntry()) == EntryType::Directory and
            !is_empty(app.getCurrentEntry())) {

            // return if the answer is not yes
            if (!confirmAction("Directory is not empty, delete it recursively? (y/n) "))
                return app.resetFooter();

            const auto deletedEntriess = fs::remove_all(app.getCurrentEntry().path());

            app.setCustomFooter([=] {
                Terminal term(Color::Green);

                term.setStyle(Style::Bold)
                    .print("Deleted entry: ", targetEntry);

                if (deletedEntriess > 1)
                    term.print(" and ", deletedEntriess - 1, " other ", (deletedEntriess > 2 ? "entries" : "entry"));
            });
        } else {
            fs::remove(app.getCurrentEntry().path());
            app.setCustomFooter([=] {
                Terminal(Color::Green).setStyle(Style::Bold).print("Deleted entry: ", targetEntry);
            });
        }

        // refresh entries
        app.updateEntries();
    } catch (...) {
        app.setCustomFooter([] {
            Terminal(Color::Red).setStyle(Style::Bold).print("Failed to delete entry!");
        });
    }
}

void InputHandler::handleTogglePreview() const {
    app.setShowPreview(!app.shouldShowPreview());
}

void InputHandler::handleToggleSortByTime() const {
    if (app.getSortType() != SortType::Time) {
        app.setSortType(SortType::Time);
    } else {
        app.setSortType(SortType::Normal);
    }
}

void InputHandler::handleToggleSortBySize() const {
    if (app.getSortType() != SortType::Size) {
        app.setSortType(SortType::Size);
    } else {
        app.setSortType(SortType::Normal);
    }
}

void InputHandler::handleMakeDirectory() const {
    std::string inputBuffer;

    // return if user cancelled
    if (!readInputString("Enter directory name: ", inputBuffer, EntryType::Directory))
        return;

    if (fs::create_directory(fs::path(inputBuffer))) {
        app.setCustomFooter([=] {
            Terminal(Color::Green).setStyle(Style::Bold).print("Created directory: ", inputBuffer);
        });
        app.updateEntries();
    } else {
        app.setCustomFooter([] {
            Terminal(Color::Red).setStyle(Style::Bold).print("Failed to create directory!");
        });
    }
}

void InputHandler::handleToggleHideEntries() const {
    app.setShowHiddenEntries(!app.shouldShowHiddenEntries());
    app.updateEntries();
}

void InputHandler::handleQuit() const {
    app.quit();
}

[[nodiscard]] Action InputHandler::getAction(const char& input) {
    if (const auto it = keyMap.find(input); it != keyMap.end())
        return it->second;
    return Action::None;
}

bool InputHandler::confirmAction(const std::string& prompt, const Color& color) const {
    Terminal::showCursor();

    app.setCustomFooter([&] {
        Terminal(color).setStyle(Style::Bold).print(prompt);
    });

    const char answer = Terminal::getChar();
    Terminal::hideCursor();

    // return if the answer is not yes
    return answer == 'y' or answer == 'Y';
}

// gets input from the user interactively into the buffer
// return true on success false if user cancelled
bool InputHandler::readInputString(const std::string& prompt, std::string& inputBuffer,
                                   const EntryType entryType) const {
    Terminal::showCursor();

    app.setCustomFooter([&] {
        Terminal(FileProperties::getColor(entryType))
                .print(prompt, inputBuffer);
    });

    bool isTakingInput = true;
    while (isTakingInput) {
        switch (const char c = Terminal::getChar()) {
            case keyCode::Enter:
                isTakingInput = false;
                break;
            case keyCode::Backspace:
                inputBuffer.pop_back();
                app.updateUI();
                break;
            case keyCode::Esc: // cancel
                Terminal::hideCursor();
                app.resetFooter();
                return false;
            default:
                // if it's a printable character add it to the buffer
                if (isprint(c)) {
                    inputBuffer.push_back(c);
                    app.updateUI();
                }
        }
    }

    Terminal::hideCursor();
    return true;
}

InputHandler& InputHandler::getInstance() {
    if (instance == nullptr)
        instance = new InputHandler();
    return *instance;
}

void InputHandler::handleInput() {
    terminal.nonBlock(
        [&]() {
            int iterations = 0;

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
                        iterations = 0;
                        break;
                    case Action::Delete:
                        handleDelete();
                        iterations = 0;
                        break;
                    case Action::MakeDirectory:
                        handleMakeDirectory();
                        iterations = 0;
                        break;
                    case Action::ToggleSortByTime:
                        handleToggleSortByTime();
                        break;
                    case Action::ToggleSortBySize:
                        handleToggleSortBySize();
                        break;
                    case Action::ToggleHideEntries:
                        handleToggleHideEntries();
                        break;
                    case Action::TogglePreview:
                        handleTogglePreview();
                        break;
                    case Action::Quit:
                        handleQuit();
                        break;
                    default:
                        break;
                }

                // reset footer after taking input
                if (++iterations >= 2) {
                    app.resetFooter();
                    app.updateUI();
                    iterations = 0;
                }
            }
        }
    );
}

InputHandler* InputHandler::instance = nullptr;
