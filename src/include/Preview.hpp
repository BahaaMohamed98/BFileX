#ifndef PREVIEW_HPP
#define PREVIEW_HPP

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

public:
    Preview();
    void resize(const int& width, const int& height);
    void render(const std::string& filePath);
};

#endif //PREVIEW_HPP
