#include "FilePreview.hpp"
#include <fstream>

void FilePreview::readFile(const std::string& path) {
    lines.clear(); // clear previous file

    std::ifstream file(path);
    if (not file.is_open()) {
        // display an error message on failure
        lines.emplace_back(("Failed to open file: " + path).substr(0, maxLineWidth));
        return;
    }

    std::string line;
    // read upto `maxLines` from the file
    for (int i = 0; getline(file, line) and i < maxLines; ++i) {
        // trimming line if it exceeds `maxLineWidth`
        if (line.size() > maxLineWidth)
            line = line.substr(0, maxLineWidth);

        lines.emplace_back(line); // store the extracted line
    }
}

void FilePreview::printBorderLine(const std::string& leftCorner, const std::string& rightCorner,
                                  const int length) const {
    Printer printer;

    // left corner
    printer.print(leftCorner);

    // horizontal line
    for (int i = 0; i < length; ++i)
        printer.print(horizontalLine);

    // right corner
    printer.println(rightCorner);
}

FilePreview::FilePreview() {
    auto [width,height] = Terminal::size();
    resize(width, height);
}

void FilePreview::resize(const int width, const int height) {
    terminalWidth = width, terminalHeight = height;

    // calculate starting position for the preview box
    leftStartingPosition = terminalWidth / 2;
    topStartingPosition = 2;

    // determine the maximum number of lines and width of each line
    maxLines = terminalHeight - 4;
    maxLineWidth = (terminalWidth - leftStartingPosition) - 3;
}

void FilePreview::render(const std::string& filePath) {
    const int contentLength = terminalWidth - leftStartingPosition - 1;

    // read the file content into the `lines` vector
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
        Cursor::moveTo(terminalWidth, topStartingPosition + i + 1);
        printer.print(verticalLine);
    }

    // move to bottom border location
    Cursor::moveTo(leftStartingPosition, terminalHeight - 1);

    // bottom border
    printBorderLine(bottomLeftCorner, BottomRightCorner, contentLength);
}

void FilePreview::clearPreview() const {
    const int contentLength = terminalWidth - leftStartingPosition + 1;
    const std::string emptyString(contentLength, ' ');

    Printer printer;
    for (int i = 0; i < terminalHeight - 2; ++i) {
        Cursor::moveTo(leftStartingPosition, topStartingPosition + i);
        printer.print(emptyString);
    }
}
