#include "Preview.hpp"
#include <fstream>

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
        // trimming the part more than maxLineWidth
        if (line.size() > maxLineWidth)
            line = line.substr(0, maxLineWidth);

        lines.emplace_back(line);
    }
}

void Preview::printBorderLine(const std::string& leftCorner, const std::string& rightCorner, const int& length) const {
    Printer printer;

    // left corner
    printer.print(leftCorner);

    // horizontal line
    for (int i = 0; i < length; ++i)
        printer.print(horizontalLine);

    // right corner
    printer.println(rightCorner);
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

    Printer printer;
    // move to starting position of the preview
    Cursor::moveTo(leftStartingPosition, topStartingPosition);

    // top border
    printBorderLine(topLeftCorner, topRightCorner, contentLength);

    // vertical borders and content
    for (int i = 0; i < maxLines; ++i) {
        Cursor::moveTo(leftStartingPosition, topStartingPosition + i + 1);

        // left vertical line
        printer.print(verticalLine);

        // file content
        if (i < lines.size())
            printer.print(" ", lines[i]);

        // right vertical line
        Cursor::moveTo(tWidth, topStartingPosition + i + 1);
        printer.print(verticalLine);
    }

    // move to bottom border location
    Cursor::moveTo(leftStartingPosition, tHeight - 1);

    // bottom border
    printBorderLine(bottomLeftCorner, BottomRightCorner, contentLength);
}
