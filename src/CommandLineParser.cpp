#include "CommandLineParser.hpp"
#include "Terminal++.hpp"

void CommandLineParser::CommandLinePrinter::printUsage() {
    Printer().println("BFileX [OPTIONS]\n");
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

void CommandLineParser::CommandLinePrinter::printCommand(const std::string& command, const std::string& description,
                                                         const bool trailingNewLine) {
    Printer printer;
    printer.setTextColor(Color::Cyan).println(std::setw(indentWidth), command)
           .resetColors().println(std::setw(indentWidth + 7), description);

    if (trailingNewLine)
        printer.println();
}

void CommandLineParser::CommandLinePrinter::printErrorUnknownCommand(const std::string& command) {
    Printer(Color::Red).print("Error: ");
    Printer().print("Unknown command: \"")
             .setTextColor(Color::Cyan).print(command).resetColors().println("\"\n");

    Printer().print("For for more information try ").setTextColor(Color::Cyan).println("--help");
}

Action CommandLineParser::getAction(const std::string& command) {
    if (const auto it = commandToAction.find(command); it != commandToAction.end()) {
        return it->second;
    }
    return Action::None;
}

void CommandLineParser::parse(const int argc, char** argv) {
    // return if no arguemnts provided
    if (argc == 1)
        return;

    App& app = App::getInstance();

    // starting at index 1 to get rid of the program name
    for (int i = 1; i < argc; ++i) {
        switch (const std::string command{argv[i]}; getAction(command)) {
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
            case Action::ToggleHelp:
                CommandLinePrinter::printHelp();
                exit(EXIT_SUCCESS);
            default:
                CommandLinePrinter::printErrorUnknownCommand(command);
                exit(EXIT_FAILURE);
        }
    }
}
