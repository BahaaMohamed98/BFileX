
# Terminal++

**Terminal++** is a simple terminal utility that provides various functionalities for text output and cursor manipulation in terminal applications. It is cross-platform, working on both Windows and UNIX-like systems, and leverages ANSI escape codes for coloring and formatting.

## Features

- Clear the terminal screen.
- Move the cursor to specific positions.
- Print text in different colors.
- Print bold text.
- Get the terminal size.
- Hide and show the cursor.
- Read a single character without waiting for the Enter key.
- Sleep for a specified number of milliseconds.
- Run tasks asynchronously without blocking the main thread.
- Support for handling keyboard inputs.

## Examples

#### Clear the Screen

To clear the terminal screen, use the following:

```cpp
Terminal::clearScreen();
```

#### Move the Cursor

To move the cursor to a specific position (x, y):

```cpp
Terminal::moveTo(1, 1); // Move to the top-left corner
```

#### Print Text with Colors

You can print text using different colors. Here's how to print "Hello, Terminal++!" in green:

```cpp
Terminal terminal;
terminal.setColor(Terminal::Color::Green);
terminal.println("Hello, Terminal++!");
```

#### Print Bold Text

To print bold text, you can do the following:

```cpp
terminal.setColor(Terminal::Color::Cyan, true);
terminal.println("This is bold cyan text!");
```

#### Get Terminal Size

You can retrieve the current size of the terminal with:

```cpp
auto [width, height] = terminal.size();
terminal.println("Terminal Size: ", width, "x", height);
```

#### Hide and Show the Cursor

To hide the cursor:

```cpp
Terminal::hideCursor();
```

And to show it again:

```cpp
Terminal::showCursor();
```

#### Read a Single Character

To read a single character without needing to press Enter:

```cpp
char input = Terminal::getChar();
terminal.println("You pressed: ", input);
```

#### Sleep for a Specified Duration

To pause execution for a certain number of milliseconds:

```cpp
Terminal::sleep(3000); // Sleep for 3 seconds
terminal.println("Awoke after 3 seconds!");
```

#### Run Tasks Asynchronously

You can run tasks in the background without blocking the main thread:

```cpp
terminal.nonBlock([]() {
    Terminal threadTerminal; // Create a new Terminal instance for this thread
    threadTerminal.setColor(Terminal::Color::Red);
    threadTerminal.println("Asynchronous task completed after 2 seconds.");
    Terminal::sleep(2000);
    threadTerminal.println("This message comes from the asynchronous thread.");
});
```

## Supported Colors

The following colors are available:

- **Reset**: `Color::Reset` (to reset to default color)
- **Red**: `Color::Red`
- **Green**: `Color::Green`
- **Yellow**: `Color::Yellow`
- **Blue**: `Color::Blue`
- **Magenta**: `Color::Magenta`
- **Cyan**: `Color::Cyan`
- **White**: `Color::White`

## Supported keyboard Buttons

The following keyboard buttons are supported for handling inputs:

- **Backspace**: `keyCode::Backspace`
- **Enter**: `keyCode::Enter`
- **Escape**: `keyCode::Esc`
- **Tab**: `keyCode::Tab`
- **Space**: `keyCode::Space`

## Compiling

To compile your code with the `Terminal++` utility, use the following command:

```bash
g++ -o example example.cpp
```

### Conclusion

Terminal++ is a straightforward and efficient way to enhance your terminal applications with various functionalities and color support. Enjoy building your terminal-based applications!
