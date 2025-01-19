#pragma once
#include "InputHandler.hpp"

class CommandLineParser {
    class CommandLinePrinter {
        static constexpr int indentWidth = 10; // Alignment width for printed commands

        static void printCommand(std::string_view command, std::string_view description, bool trailingNewLine = true);

    public:
        static void printUsage();
        static void printHelp();
        static void printErrorUnknownCommand(std::string_view command); // Print error for unrecognized commands
        static void printErrorMultipleEntries();                        // Print error for unrecognized commands
    };

    static inline std::unordered_map<std::string_view, Action> commandToAction{
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

    static Action getAction(std::string_view argument);

public:
    static void parse(int argc, char** argv);
};
