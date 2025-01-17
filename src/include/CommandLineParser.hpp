#pragma once
#include "InputHandler.hpp"

class CommandLineParser {
    class CommandLinePrinter {
        static constexpr int indentWidth = 10; // Alignment width for printed commands

        static void printCommand(const std::string& command, const std::string& description,
                                 bool trailingNewLine = true);

    public:
        static void printUsage();
        static void printHelp();
        static void printErrorUnknownCommand(const std::string& command); // Print error for unrecognized commands
        static void printErrorMultipleEntries(); // Print error for unrecognized commands
    };

private:
    static inline std::unordered_map<std::string, Action> commandToAction{
        {"-t", Action::ToggleSortByTime},
        {"--time", Action::ToggleSortByTime},

        {"-s", Action::ToggleSortBySize},
        {"--size", Action::ToggleSortBySize},

        {"-r", Action::ToggleReverseEntries},
        {"--reverse", Action::ToggleReverseEntries},

        {"-a", Action::ToggleHideEntries},
        {"--all", Action::ToggleHideEntries},

        {"-np", Action::TogglePreview},
        {"--no-preview", Action::TogglePreview},

        {"-h", Action::ToggleHelp},
        {"--help", Action::ToggleHelp},
    };

    static Action getAction(const std::string& argument);
public:
    static void parse(int argc, char** argv);
};
