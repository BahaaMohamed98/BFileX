#pragma once

#include "Terminal++.hpp"

class Preview {
    int tWidth{};
    int tHeight{};
    int maxLines{};
    int maxLineWidth{};
    int leftStartingPosition{};
    int topStartingPosition{};

    const std::string
            topLeftCorner{"╭"},
            topRightCorner{"╮"},
            bottomLeftCorner{"╰"},
            BottomRightCorner{"╯"},
            verticalLine{"│"},
            horizontalLine{"─"};

    std::vector<std::string> lines;

    void readFile(const std::string& path);
    void printBorderLine(const std::string& leftCorner, const std::string& rightCorner, const int& length) const;

public:
    Preview(); // todo: refactor to take width and height
    void resize(const int& width, const int& height);
    void render(const std::string& filePath);
};
