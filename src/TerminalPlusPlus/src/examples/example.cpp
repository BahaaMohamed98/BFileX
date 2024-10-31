#include "Terminal++.hpp"

int main() {
    // Create a Terminal object to work with
    Terminal terminal;

    // Clear the terminal screen
    Terminal::clearScreen();

    // Get and display the terminal size
    auto [width, height] = Terminal::size();
    terminal.println("Terminal Size: ", width, " x ", height);

    // Set the text color to green and print a message
    terminal.setTextColor(Color::Green);
    terminal.println("Welcome to Terminal++!");

    // Set color to blue and print another message
    terminal.setTextColor(Color::Blue);
    terminal.println("This library supports various colors.");

    // Set color to yellow and bold
    terminal.setTextColor(Color::Yellow, true);
    terminal.println("You can also print bold text!");

    // Reset to normal color
    terminal.setTextColor(Color::Reset);

    // Print a message indicating how to use the library
    terminal.println("Use Terminal::clearScreen() to clear the screen.");
    terminal.println("Use Terminal::getChar() to read a character.");

    // Move the cursor to a specific position (10, 5) and print a message
    Terminal::moveTo(10, 5);
    terminal.println("Cursor moved to (10, 5)!");

    // Hide the cursor
    Terminal::hideCursor();
    terminal.println("The cursor is now hidden.");

    // Demonstrating sleep function
    terminal.println("Sleeping for 3 seconds...");
    Terminal::sleep(3000); // Sleep for 3000 milliseconds (3 seconds)
    terminal.println("Awoke after 3 seconds!");

    // Using nonBlock to run a task asynchronously with a separate Terminal instance
    terminal.nonBlock([]() {
        Terminal threadTerminal; // Create a new Terminal instance for this thread
        threadTerminal.setTextColor(Color::Red); // Set color for this thread
        threadTerminal.println("Asynchronous task completed after 2 seconds.");
        Terminal::sleep(2000); // Sleep for 2 seconds in a separate thread
        threadTerminal.println("This message comes from the asynchronous thread.");
    });

    // Additional nonBlock example with separate Terminal instance
    terminal.nonBlock([]() {
        Terminal threadTerminal; // Create a new Terminal instance for this thread
        threadTerminal.setTextColor(Color::Cyan); // Set color for this thread
        for (int i = 1; i <= 5; ++i) {
            Terminal::sleep(1000); // Sleep for 1 second
            threadTerminal.println("Count from non-blocking task: ", i);
        }
    });

    // Continue interacting with the terminal while the non-blocking tasks run
    terminal.println("You can keep using the terminal while tasks run in the background.");

    // Show the cursor again
    Terminal::showCursor();
    terminal.println("The cursor is now visible again.");

    // Finish by flushing the output
    Terminal::flush();

    return 0;
}