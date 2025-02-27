#include "InputHandler.hpp"
#include <filesystem>
#include <fstream>

InputHandler::InputHandler()
    : app(App::getInstance()) {}

void InputHandler::handleUp() const {
    app.decrementCurrentEntryIndex();
}

void InputHandler::handleDown() const {
    app.incrementCurrentEntryIndex();
}

void InputHandler::handleEnter() const {
    const fs::directory_entry currentEntry = app.getCurrentEntry();

    if (currentEntry.is_directory()) {
        app.changeDirectory(fs::absolute(currentEntry));
    } else if (currentEntry.is_regular_file() and not FileProperties::Utilities::isExecutable(currentEntry.path())) {
        if (FileProperties::Utilities::isBinary(currentEntry.path().string())) {
            FileManager::openFile(fs::absolute(currentEntry));
            return;
        }

        // open the file in the user's editor
        FileManager::openFileInEditor(fs::absolute(currentEntry));

        // revert the editor's changes to the terminal
        app.initializeTerminal();
    }
}

void InputHandler::handleBack() const {
    app.changeDirectory(fs::current_path() / "..");
}

void InputHandler::handleRename() const {
    // initializing the input buffer to the current file name
    std::string inputBuffer = FileProperties::MetaData::getName(app.getCurrentEntry()).string();

    // return directly if the user tries to rename the `..` directory (do nothing)
    if (inputBuffer == "..") {
        return;
    }

    // return if user cancelled
    if (not readInputString("New name: ", inputBuffer,
                            FileProperties::Types::determineEntryType(app.getCurrentEntry()))) {
        return;
    }

    try {
        // file entry before renaming
        const fs::directory_entry oldEntry = app.getCurrentEntry();
        const fs::path newPath = fs::current_path() / fs::path(inputBuffer);

        // return if the name was not changed
        if (oldEntry.path() == newPath) {
            app.resetFooter();
            return;
        }

        // if entry already exists prompt the user about overriding it
        if (fs::exists(newPath)) {
            // return if the answer is not yes
            if (not confirmAction("Entry already exists override it? (y/n) ")) {
                app.resetFooter();
                return;
            }
        }

        // rename the entry to the specified name
        fs::rename(oldEntry.path(), newPath);

        app.setCustomFooter([=] {
            Printer(Color::Green)
                    .setTextStyle(TextStyle::Bold)
                    .print("Renamed ", FileProperties::MetaData::getName(oldEntry), " to: ", newPath.filename());
        }, false);

        // refresh the entries stored
        app.updateEntries(false);

        // set the cursor to point at the same entry after renaming
        const int newIndex = FileManager::getIndex(newPath, app.getEntries());
        app.setCurrentEntryIndex(newIndex);
    } catch (const fs::filesystem_error&) {
        app.setCustomFooter([] {
            Printer(Color::Red).setTextStyle(TextStyle::Bold).print("Failed to rename entry!");
        }, true);
    }
}

void InputHandler::handleDelete() const {
    const fs::path targetEntry = FileProperties::MetaData::getName(app.getCurrentEntry());

    // return when trying to delete `..`
    if (targetEntry.string() == "..") {
        return;
    }

    // return if the answer is not yes
    if (not confirmAction("Are you sure you want to delete \"" + targetEntry.string() + "\"? (y/n) ")) {
        app.resetFooter();
        return;
    }

    try {
        // if entry is a non empty directory prompt the user about recursively deleting it
        if (FileProperties::Types::determineEntryType(app.getCurrentEntry()) == EntryType::Directory and
            not is_empty(app.getCurrentEntry())) {
            // return if the answer is not yes
            if (not confirmAction("Directory is not empty, delete it recursively? (y/n) ")) {
                app.resetFooter();
                return;
            }

            const auto deletedEntriess = fs::remove_all(app.getCurrentEntry().path());

            app.setCustomFooter([=] {
                Printer printer(Color::Green);

                printer.setTextStyle(TextStyle::Bold)
                        .print("Deleted entry: ", targetEntry);

                if (deletedEntriess > 1) {
                    printer.print(" and ", deletedEntriess - 1, " other ", (deletedEntriess > 2 ? "entries" : "entry"));
                }
            }, false);
        } else {
            fs::remove(app.getCurrentEntry().path());
            app.setCustomFooter([=] {
                Printer(Color::Green).setTextStyle(TextStyle::Bold).print("Deleted entry: ", targetEntry);
            }, false);
        }

        // refresh entries
        app.updateEntries(true);
    } catch (const fs::filesystem_error&) {
        app.setCustomFooter([] {
            Printer(Color::Red).setTextStyle(TextStyle::Bold).print("Failed to delete entry!");
        }, true);
    }
}

void InputHandler::handleTogglePreview() const {
    app.setShowPreview(not app.shouldShowPreview());
}

void InputHandler::handleToggleSearch() const {
    std::string inputBuffer = app.getSearchQuery();

    // return if user cancelled
    if (not readInputString("Search: ", inputBuffer,
                            FileProperties::Types::determineEntryType(app.getCurrentEntry()))) {
        return;
    }

    app.resetFooter(false);
    app.setSearchQuery(inputBuffer);
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

void InputHandler::handleToggleReverseEntries() const {
    app.setReverseEntries(not app.shouldReverseEntries());
}

void InputHandler::handleMakeDirectory() const {
    std::string inputBuffer;

    // return if user cancelled
    if (not readInputString("Enter directory name: ", inputBuffer, EntryType::Directory)) {
        return;
    }

    bool directoryCreated;

    try {
        if (fs::exists(inputBuffer)) {
            app.setCustomFooter([] {
                Printer(Color::Red).setTextStyle(TextStyle::Bold).print("File already exists!");
            }, true);
            return;
        }

        directoryCreated = fs::create_directory(fs::path(inputBuffer));
    } catch (const fs::filesystem_error&) {
        directoryCreated = false;
    }

    if (directoryCreated) {
        app.setCustomFooter([=] {
            Printer(Color::Green).setTextStyle(TextStyle::Bold).print("Created directory: ", inputBuffer);
        }, false);

        app.updateEntries(false);

        // place cursor on the newly created directory
        const int newIndex = FileManager::getIndex(fs::absolute(inputBuffer), app.getEntries());
        app.setCurrentEntryIndex(newIndex);
    } else {
        app.setCustomFooter([] {
            Printer(Color::Red).setTextStyle(TextStyle::Bold).print("Failed to create directory!");
        }, true);
    }
}

void InputHandler::handleCreateFile() const {
    std::string inputBuffer;

    if (not readInputString("Enter file name: ", inputBuffer, EntryType::RegularFile)) {
        return;
    }

    if (fs::exists(inputBuffer)) {
        app.setCustomFooter([] {
            Printer(Color::Red).setTextStyle(TextStyle::Bold).print("File already exists!");
        }, true);
        return;
    }

    if (std::ofstream(inputBuffer).is_open()) {
        app.setCustomFooter([=] {
            Printer(Color::Green).setTextStyle(TextStyle::Bold).print("Created file: ", inputBuffer);
        }, false);

        app.updateEntries(false);

        // place cursor on the newly created file
        const int fileIndex = FileManager::getIndex(fs::absolute(inputBuffer), app.getEntries());
        app.setCurrentEntryIndex(fileIndex);
    } else {
        app.setCustomFooter([] {
            Printer(Color::Red).setTextStyle(TextStyle::Bold).print("Failed to create file!");
        }, true);
    }
}

void InputHandler::handleToggleHideEntries() const {
    app.setShowHiddenEntries(not app.shouldShowHiddenEntries());
    app.updateEntries(true);
}

void InputHandler::handleQuit() const {
    app.quit();
}

[[nodiscard]] Action InputHandler::getAction(const char input) {
    if (const auto it = keyMap.find(input); it != keyMap.end())
        return it->second;
    return Action::None;
}

bool InputHandler::confirmAction(const std::string_view prompt, const Color::Code& color) const {
    Cursor::show();

    app.setCustomFooter([&] {
        Printer(color).setTextStyle(TextStyle::Bold).print(prompt);
    }, true);

    const char answer = Input::getChar();
    Cursor::hide();

    // return if the answer is not yes
    return answer == 'y' or answer == 'Y';
}

// gets input from the user interactively into the buffer
// return true on success false if user cancelled
bool InputHandler::readInputString(const std::string_view prompt, std::string& inputBuffer,
                                   const EntryType entryType) const {
    Cursor::show();

    app.setCustomFooter([&] {
        Printer(FileProperties::Mapper::getColor(entryType))
                .print(prompt, inputBuffer);
    }, true);

    bool isTakingInput = true;
    while (isTakingInput) {
        switch (const char c = Input::getChar()) {
            case keyCode::Enter:
                isTakingInput = false;
                break;
            case keyCode::Backspace:
                if (not inputBuffer.empty()) {
                    inputBuffer.pop_back();
                    app.updateUI();
                }
                break;
            case keyCode::Esc: // cancel
                Cursor::hide();
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

    Cursor::hide();
    return true;
}

void InputHandler::inputLoop() const {
    int iterations = 0;

    while (app.isRunning()) {
        // reset footer after taking input
        if (++iterations >= 2) {
            app.resetFooter(false);

            iterations = 0;
        }

        switch (getAction(Input::getChar())) {
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
                iterations = 1;
                break;
            case Action::Delete:
                handleDelete();
                iterations = 1;
                break;
            case Action::MakeDirectory:
                handleMakeDirectory();
                iterations = 1;
                break;
            case Action::CreateFile:
                handleCreateFile();
                iterations = 1;
                break;
            case Action::ToggleSortByTime:
                handleToggleSortByTime();
                break;
            case Action::ToggleSortBySize:
                handleToggleSortBySize();
                break;
            case Action::ToggleReverseEntries:
                handleToggleReverseEntries();
                break;
            case Action::ToggleHideEntries:
                handleToggleHideEntries();
                break;
            case Action::TogglePreview:
                handleTogglePreview();
                break;
            case Action::ToggleSearch:
                handleToggleSearch();
                break;
            case Action::Quit:
                handleQuit();
                break;
            case Action::ESC:
                if (app.resetSearchQuery()) {
                    app.updateEntries(true);
                }
                break;
            default:
                break;
        }
    }
}

void InputHandler::handleInput() {
    const InputHandler inputHandler;
    inputHandler.inputLoop();
}
