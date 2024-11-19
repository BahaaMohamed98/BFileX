#pragma once
#include "InputHandler.hpp"

class CommandLineParser {
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

    static void printUsage();
    static void printHelp();
    static Action getAction(const std::string& command);

public:
    static void parse(int argc, char** argv);
};
