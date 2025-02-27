#pragma once

#include "../include/Terminal++/src/Terminal++.hpp"

class FilePreview {
    // terminal dimensions
    int terminalWidth{};
    int terminalHeight{};

    // maximum number of lines to display in the preview
    int maxLines{};
    // maximum width of each line
    int maxLineWidth{};

    // starting positions of the preview
    int leftStartingPosition{};
    int topStartingPosition{};

    // strings for corners and borders of the preview box
    const std::string
            topLeftCorner{"╭"},
            topRightCorner{"╮"},
            bottomLeftCorner{"╰"},
            BottomRightCorner{"╯"},
            verticalLine{"│"},
            horizontalLine{"─"};

    // lines read from the file
    std::vector<std::string> lines;

    // reads a given file's content into `lines`
    // following the maxLines and maxLineWidth constraints
    void readFile(const std::string& path);
    // prints a horizontal line border with the given corner strings
    void printBorderLine(const std::string& leftCorner, const std::string& rightCorner, int length) const;

public:
    FilePreview();
    void resize(int width, int height);       // resizes the preview
    void render(const std::string& filePath); // renders the preview of the given file
    void clearPreview() const;                // clears the preview area
};
