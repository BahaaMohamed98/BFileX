#ifndef TERMINAL_HPP
#define TERMINAL_HPP

/*
         _____                   _             _
        |_   _|__ _ __ _ __ ___ (_)_ __   __ _| |  _     _
          | |/ _ \ '__| '_ ` _ \| | '_ \ / _` | |_| |_ _| |_
          | |  __/ |  | | | | | | | | | | (_| | |_   _|_   _|
          |_|\___|_|  |_| |_| |_|_|_| |_|\__,_|_| |_|   |_|

        Author: BahaaMohamed98
 */

#include <cstdlib>
#include <functional>
#include <iostream>
#include <thread>

#ifdef _WIN32

#include <conio.h>
#include <windows.h>

#else

#include <cstdio>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#endif

// Enum for terminal text colors
enum class Color {
    Black = 30,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    Reset = 0, // Resets to the normal color
};

// Enum for keyboard buttons' keyCodes
enum keyCode {
#ifdef _WIN32
    Backspace = 8,
    Enter     = 13,
#else
    Backspace = 127,
    Enter     = 10,
#endif
    Esc   = 27,
    Tab   = 9,
    Space = 32,
};

// Options for clearScreen()
// "All" for a complete clear including history
// "Purge" for clearing the visible screen while preserving scrollback
// "Line" for clearing just the current line
enum ClearType {
    All,   // clear all plus history
    Purge, // clear the screen leaving history
    Line,  // clear the current line
};

class Terminal {
    Color textColor;       // Holds the current text color
    bool isBoldColor;      // Indicates if the current text is bold
    Color backgroundColor; // Holds the current background color

    struct TerminalSize {
        int width;
        int height;

        bool operator!=(const TerminalSize& other) const {
            return width != other.width or height != other.height;
        }
    };

    TerminalSize dimensions{};
    std::vector<std::thread> threads;

    // Converts a color and boldness state to ANSI escape codes
    static std::string textColorToString(const Color& color, const bool& isBold = false) {
        if (color == Color::Reset)
            return "\033[0m";
        return "\033[" + std::string{isBold ? "1" : "22"} + ";" + std::to_string(static_cast<int>(color)) + "m";
    }

    static std::string backgroundColorToString(const Color& color) {
        if (color == Color::Reset)
            return textColorToString(color);
        return "\033[" + std::to_string(static_cast<int>(color) + 10) + "m";
    }

public:
    explicit Terminal(const Color& textColor = Color::Reset, const Color& backgroundColor = Color::Reset)
        : textColor(textColor), isBoldColor(false), backgroundColor(backgroundColor) {
        dimensions = size();
    }

    // Destructor ensures that all spawned threads are joined before the object is destroyed
    // to prevent potential crashes from detached threads running after the object is deleted
    ~Terminal() {
        awaitCompletion();
    }

    // Waits for all non-blocking tasks to finish before continuing
    // Call this after starting tasks with nonBlock()
    Terminal& awaitCompletion() {
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        threads.clear(); // Clear the threads vector after joining
        return *this;
    }

    // returns terminal size struct of (width, height)
    static TerminalSize size() {
        TerminalSize size{0, 0};

#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            size.width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            size.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        }
#else
        winsize w{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
            size.height = w.ws_row;
            size.width = w.ws_col;
        }
#endif

        return size;
    }

    // returns true if the terminal was resized
    // should be called from within a loop
    [[nodiscard]] bool isResized() {
        const auto ndimensions = size();

        const bool resized = ndimensions != dimensions;
        dimensions = ndimensions;

        return resized;
    }

    // returns true if the terminal was resized
    // should be called using nonBlock as it blocks the thread
    // should be called from within a loop
    [[nodiscard]] static bool keyPressed() {
#ifdef _WIN32
        return _kbhit();  // Windows
#else
        termios oldt{}, newt{};
        int bytesAvailable = 0;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ioctl(STDIN_FILENO, FIONREAD, &bytesAvailable);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return bytesAvailable > 0; // Linux/macOS
#endif
    }

    // Clears the terminal screen
    static void clearScreen(const ClearType& cleartype = ClearType::All) {
        switch (cleartype) {
            case All:
#ifdef _WIN32 // For Windows
                system("cls");
#else // for linux and macOS
                system("clear");
#endif
                break;
            case Purge:
                std::cout << "\033[2J" << std::flush;
                break;
            case Line:
                std::cout << "\33[2K\r" << std::flush;
                break;
        }
    }

    // Sleeps for specified number of milliseconds
    static void sleep(const int& milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    // Runs a given lambda function on a separate thread
    Terminal& nonBlock(const std::function<void()>& task) {
        // Adds a new thread to the threads vector for the task to be executed asynchronously
        // This ensures that the thread can be joined later when the Terminal object is destroyed
        threads.emplace_back(task);
        return *this;
    }

    // Moves the cursor to the specified (x, y) position in the terminal
    // starting from (1, 1) at the top left corner of the terminal
    static void moveTo(const int& x, const int& y) {
        std::cout << "\033[" << y << ";" << x << "H";
    }

    // Prints multiple arguments to the terminal
    template<class T>
    Terminal& print(const T& arg) {
        std::cout << backgroundColorToString(backgroundColor);

        if (textColor != Color::Reset) // if color is not reset then set it as it will affect the background color
            std::cout << textColorToString(textColor, isBoldColor);

        std::cout << arg
                << backgroundColorToString(Color::Reset); // reset colors again

        return *this;
    }

    template<typename T, typename... Args>
    Terminal& print(const T& arg, const Args&... args) {
        print(arg);
        print(args...);
        return *this;
    }

    Terminal& println() {
        print("\n");
        return *this;
    }

    // Prints multiple arguments followed by a newline
    template<typename... Args>
    Terminal& println(const Args&... args) {
        print(args...);
        print("\n");
        return *this;
    }

    // Flushes the output stream
    static void flush() {
        std::cout.flush();
    }

    static void hideCursor() {
        std::cout << "\033[?25l";
    }

    static void showCursor() {
        std::cout << "\033[?25h";
    }

    // Sets the current text color and boldness
    Terminal& setTextColor(const Color& textColor, const bool& isBold = false) {
        this->textColor = textColor;
        isBoldColor = isBold;
        return *this;
    }

    Terminal& setBackgroundColor(const Color& backgroundColor) {
        this->backgroundColor = backgroundColor;
        return *this;
    }

    // Reads a single character from the terminal unbuffered input
    static char getChar() {
#ifdef _WIN32
        return getch();
#else
        termios oldattr{}, newattr{};
        tcgetattr(STDIN_FILENO, &oldattr); // Get current terminal attributes
        newattr = oldattr;
        newattr.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echoing
        tcsetattr(STDIN_FILENO, TCSANOW, &newattr); // Apply new attributes immediately
        const int ch = getchar();                   // Read a single character
        tcsetattr(STDIN_FILENO, TCSANOW, &oldattr); // Restore original attributes
        return static_cast<char>(ch);
#endif
    }

    // gets a string from stdin
    static std::string getString(const std::string& prompt = "") {
        std::cout << prompt;
        std::string str;

        std::cin >> str;
        std::cin.ignore();
        return str;
    }

    // gets a full line from stdin
    static std::string getLine(const std::string& prompt = "") {
        std::cout << prompt;
        std::string line;

        getline(std::cin, line, '\n');
        return line;
    }
};

#endif //TERMINAL_HPP
