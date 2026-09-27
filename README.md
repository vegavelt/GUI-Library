# GUI Project

This repository contains the project completed at Ensimag. All of our implementations are in the `implem` folder. The files in the `api` folder and some of the test files were provided and were not written by us. The headers of files that were not created by us include the relevant credits.

![Intro](img/intro.png)

This project focuses on implementing the `libei.a` library using the provided API. The library allows developers to build custom graphical user interfaces (GUIs) by offering core widget types such as `Toplevel`, `Frame`, and `Button`. It also enables the creation of custom widgets, the registration of button callback functions, and the integration of custom event handlers.

## Project Structure

The main project structure is as follows:

```
project/
|-- api/            # API files
|-- docs/           # Doxygen documentation describing the implemented functions
|-- implem/         # Source files
|-- misc/           # Assets (images, fonts, etc.)
|-- CMakeLists.txt  # CMake build script that defines the compilation rules
|-- README.md       # Project introduction
```

## Implemented Features

- Drawing of graphic primitives (lines, polygons)
- Custom widget structure and inheritance system
- Three widget classes inheriting from the base widget class (`Toplevel`, `Frame`, `Button`)
- Geometry manager for placing widgets and their children at specific locations
- Event handling (mouse clicks, key presses, etc.)


## How to Compile

The project archive contains a `CMakeLists.txt` file. To compile outside the project source tree, use the provided build directory or create a dedicated build folder. For example:

```bash
mkdir build
cd build
cmake ..
make minimal
```

This generates the executable `minimal`.

It is also recommended to use an IDE such as CLion for easier project launching and compilation.

## How to Run

After compilation, the generated executable can be run with:

```bash
./name_of_executable
```

## Dependencies

- Standard C libraries
- SDL2

## Build Requirements

- CMake version 3.20 or later
- A C compiler (e.g., GCC or Clang)
- Make (if not using an IDE such as CLion)

## Tests

Several example programs are provided, including `minimal.c`, `lines.c`, `button.c`, `hello_world.c`, `puzzle.c`, `minesweeper.c`, and `two048.c`. Some of them are mini-games built using the library. They serve as examples of how to use the library to create GUIs. To compile and run a specific test, for example `minimal.c`, use:

```bash
make minimal
./minimal
```

### minimal

This test fills the root frame with two colors (red at the top and white at the bottom), verifying the `ei_fill()` function that fills a rectangle with a given color.

### lines

This test draws lines, rectangles, and polygons and fills them with color, verifying `ei_fill()`, `ei_draw_polyline()`, `ei_draw_polygon()`, and the clipping algorithm.

### button

This test displays a clickable button that changes color when clicked and returns to its original color when the cursor leaves. It verifies button text display, event handling, and callback functions.

### hello_world

This test shows a toplevel window with two buttons (close and clickable), verifying widget destruction, dragging, resizing, and optionally bringing a clicked toplevel to the front (with the extra test block to uncomment).

### minesweeper

This test implements the classic Minesweeper game, verifying left/right mouse event handling, image manipulation on buttons, and a timer that updates every second.

![Minesweeper](img/minesweeper.png)

### puzzle

This test implements the 15 Puzzle game, verifying `ei_copy_surface()`, transparent surfaces, moving widgets with children, and keyboard shortcuts (`Ctrl+W` to close, `Ctrl+N` to open a new game).

![Puzzle](img/puzzle.png)

### two048

This test implements the 2048 game, verifying keyboard event handling, window updates after each move, and shortcuts (`Ctrl+W` to close, `Ctrl+N` to open a new game).

![2048](img/2048.png)

### ext_testclass

This test demonstrates how to add a new widget type to the library, allowing custom widget extensions by the programmer.

## Authors

-   **Alexandre Guy**
-   **Tomas Vega Velasquez**
-   **You Chen Koh**

## License

This project is intended for academic use and demonstration purposes only.
