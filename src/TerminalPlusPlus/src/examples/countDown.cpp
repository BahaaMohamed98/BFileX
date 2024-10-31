#include "Terminal++.hpp"

void countdown(const int seconds) {
    Terminal terminal; // Create a Terminal object for this thread
    terminal.setTextColor(Color::Cyan); // Set text color to Cyan
    Terminal::clearScreen();
    Terminal::hideCursor(); // hiding the cursor

    terminal.println("Starting countdown...");
    Terminal::sleep(1000);
    Terminal::clearScreen();

    // Loop through the countdown
    for (int i = seconds; i > 0; --i) {
        Terminal::moveTo(1, 1); // Move cursor to the top-left corner
        terminal.println("Countdown: ", i, " seconds remaining...");

        Terminal::flush(); // Flush the output stream
        Terminal::sleep(1000); // Sleep for 1 second
    }

    Terminal::clearScreen();
    Terminal::showCursor();

    Terminal::moveTo(1, 1); // Move cursor to the top-left corner
    terminal.setTextColor(Color::Green); // Change color to Green for completion
    terminal.println("Time's up!"); // Print completion message
}

int main() {
    Terminal::clearScreen(); // Clear the terminal screen

    Terminal terminal; // Create a Terminal object for the main thread
    terminal.println("Welcome to the Countdown Timer!");
    terminal.print("Enter the countdown time in seconds: ");

    int seconds;
    std::cin >> seconds; // Get user input for countdown time
    std::cin.ignore(); // Ignore the newline character after input

    // Start the countdown in a separate thread
    terminal.nonBlock([&]() {
        countdown(seconds);
    });

    // wait for the countdown to finish
    terminal.awaitCompletion();

    // Main thread can do other things or just wait
    terminal.println("Press any key to exit...");

    // Wait for user input before exiting
    Terminal::getChar();

    Terminal::clearScreen();
    terminal.println("Exiting the timer. Goodbye!");
}
