#include "Preview.hpp"
#include <fstream>
#include "FileProperties.hpp"

void Preview::readFile(const std::string& path) {
    lines.clear(); // clear previous file

    std::ifstream file(path);
    if (!file.is_open()) {
        // on failure write failed to open in the preview
        lines.emplace_back(("Failed to open file: " + path).substr(0, maxLineWidth));
        return;
    }

    std::string line;
    // read maxLines number of lines or less
    for (int i = 0; getline(file, line) and i < maxLines; ++i) {
        // triming the part more than maxLineWidth
        if (line.size() > maxLineWidth)
            line = line.substr(0, maxLineWidth);

        lines.emplace_back(line);
    }
}

Preview::Preview() {
    auto [width,height] = Terminal::size();
    resize(width, height);
}

void Preview::resize(const int& width, const int& height) { // todo: change to a simpler way
    tWidth = width, tHeight = height;
    leftStartingPosition = tWidth / 2;
    topStartingPosition = 2;
    maxLines = tHeight - 4;
    maxLineWidth = (tWidth - leftStartingPosition) - 3;
}

void Preview::render(const std::string& filePath) {
    const int contentLength = tWidth - leftStartingPosition - 1;

    // read file's content
    readFile(filePath);

    Terminal terminal;
    // move to preview location
    Terminal::moveTo(leftStartingPosition, topStartingPosition);

    // top left corner
    terminal.print(topLeftCorner);

    // top horizontal line
    for (int i = 0; i < contentLength; ++i)
        terminal.print(horizontalLine);

    // top right corner
    terminal.println(topRightCorner);

    // vertical borders and content
    for (int i = 0; i < maxLines; ++i) {
        Terminal::moveTo(leftStartingPosition, topStartingPosition + i + 1);

        // left vertical line
        terminal.print(verticalLine);

        // file content
        if (i < lines.size())
            terminal.print(" ", lines[i]);

        // right vertical line
        Terminal::moveTo(tWidth, topStartingPosition + i + 1);
        terminal.print(verticalLine);
    }

    Terminal::moveTo(leftStartingPosition, tHeight - 1);
    // bottom left corner
    terminal.print(bottomLeftCorner);

    // bottom horizontal line
    for (int i = 0; i < contentLength; ++i)
        terminal.print(horizontalLine);

    // bottom right corner
    terminal.print(BottomRightCorner);
}
