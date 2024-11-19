#include "CommandLineParser.hpp"

void CommandLineParser::printUsage() {
    Terminal().println("BFileX [OPTIONS]\n");
}

void CommandLineParser::printHelp() {
    Terminal terminal;
    constexpr int width = 10;

    terminal.setTextColor(Color::Blue).println("\nBFileX")
            .resetColors().println(std::setw(width), "A simple terminal-based file explorer\n");

    terminal.setTextColor(Color::Yellow).print("USAGE:\n", std::setw(width)).resetColors();
    printUsage();

    terminal.setTextColor(Color::Yellow).println("OPTIONS:");

    auto printCommand = [&](const std::string& command, const std::string& description) {
        terminal.setTextColor(Color::Cyan).println(std::setw(width), command)
                .resetColors().println(std::setw(width + 7), description, "\n");
    };

    printCommand("-t, --time", "Sort entries by time");
    printCommand("-s, --size", "Sort entries by size");
    printCommand("-r, --reverse", "Reverse entries");
    printCommand("-a, --all", "Show all entries");
    printCommand("-np, --no-preview", "Don't show file preview");
    printCommand("-h, --help", "Show help screen");
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

    std::stringstream commands;
    // reading the commands into a string stream
    // starting at index 1 to get rid of the program name
    for (int i = 1; i < argc; ++i) {
        commands << argv[i] << ' ';
    }

    App& app = App::getInstance();
    std::string command;

    for (int i = 1; i < argc; ++i) {
        // getting the next command
        commands >> command;

        switch (getAction(command)) {
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
                printHelp();
                exit(0);
            default:
                Terminal(Color::Red).print("Error: ");
                Terminal().print("Unknown command: \"")
                          .setTextColor(Color::Cyan).print(command).resetColors().println("\"\n");

                Terminal().print("For for more information try ").setTextColor(Color::Cyan).println("--help");
                exit(1);
        }
    }
}
