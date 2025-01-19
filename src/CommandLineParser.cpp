#include "CommandLineParser.hpp"
#include "Terminal++.hpp"

void CommandLineParser::CommandLinePrinter::printUsage() {
    Printer().println("BFileX <path> [OPTIONS]\n");
}

void CommandLineParser::CommandLinePrinter::printHelp() {
    Printer printer;

    printer.setTextColor(Color::Blue).println("\nBFileX")
            .resetColors().println(std::setw(indentWidth), "A simple terminal-based file explorer\n");

    printer.setTextColor(Color::Yellow).print("USAGE:\n", std::setw(indentWidth)).resetColors();
    printUsage();

    printer.setTextColor(Color::Yellow).println("OPTIONS:");

    printCommand("-t, --time", "Sort entries by time");
    printCommand("-s, --size", "Sort entries by size");
    printCommand("-r, --reverse", "Reverse entries");
    printCommand("-a, --all", "Show all entries");
    printCommand("-np, --no-preview", "Don't show file preview");
    printCommand("-h, --help", "Show help screen", false);
}

void CommandLineParser::CommandLinePrinter::printCommand(const std::string_view command,
                                                         const std::string_view description,
                                                         const bool trailingNewLine) {
    Printer printer;
    printer.setTextColor(Color::Cyan).println(std::setw(indentWidth), command)
            .resetColors().println(std::setw(indentWidth + 7), description);

    if (trailingNewLine)
        printer.println();
}

void CommandLineParser::CommandLinePrinter::printErrorUnknownCommand(const std::string_view command) {
    Printer(Color::Red).print("Error: ");
    Printer().print("Unknown command: \"")
            .setTextColor(Color::Cyan).print(command).resetColors().println("\"\n");

    Printer().print("For for more information try ").setTextColor(Color::Cyan).println("--help");
}

void CommandLineParser::CommandLinePrinter::printErrorMultipleEntries() {
    Printer(Color::Red).println("Error: only one entry allowed");
}

Action CommandLineParser::getAction(const std::string_view argument) {
    // if the argument doesn't start with a dash it should be a path
    if (argument.front() != '-') {
        return Action::SetStartingDirectory;
    }

    if (const auto it = commandToAction.find(argument); it != commandToAction.end()) {
        return it->second;
    }

    return Action::None;
}

void CommandLineParser::parse(const int argc, char** argv) {
    // return if no arguemnts provided
    if (argc == 1)
        return;

    App& app = App::getInstance();
    bool changedStartingDirectory{};

    // starting at index 1 to get rid of the program name
    for (int i = 1; i < argc; ++i) {
        switch (const std::string_view argument{argv[i]}; getAction(argument)) {
            case Action::ToggleSortByTime:
                app.setSortType(SortType::Time);
                break;
            case Action::ToggleSortBySize:
                app.setSortType(SortType::Size);
                break;
            case Action::ToggleReverseEntries:
                app.setReverseEntries(true);
                break;
            case Action::ToggleHideEntries:
                app.setShowHiddenEntries(true);
                break;
            case Action::TogglePreview:
                app.setShowPreview(false);
                break;
            case Action::SetStartingDirectory:
                if (!changedStartingDirectory) {
                    app.setStartingEntry(argument);
                    changedStartingDirectory = true;
                } else {
                    CommandLinePrinter::printErrorMultipleEntries();
                    exit(EXIT_FAILURE);
                }
                break;
            case Action::ToggleHelp:
                CommandLinePrinter::printHelp();
                exit(EXIT_SUCCESS);
            default:
                CommandLinePrinter::printErrorUnknownCommand(argument);
                exit(EXIT_FAILURE);
        }
    }
}
