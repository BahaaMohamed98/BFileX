# BFileX

<div align="center">
  <img src="assets/logo.svg" alt="Terminal++ Logo" width="600" style="margin: 20px 0"/>
</div>

A modern, lightweight terminal file explorer written in C++. BFileX provides an intuitive interface for navigating and
managing your files with vim-style keybindings and a color-coded display.

## ✨ Features

- 🎨 Color-coded interface for different file types
- 𝐕I Vim-style navigation
- 🚀 Fast and lightweight
- 🔍 File type detection with custom icons
- 📊 Detailed file information (permissions, size, modified date)
- 🖥️ Cross-platform support (Linux, macOS, Windows)
- 🔄 Real-time updates for file system changes
- ⬆️ Comprehensive keybindings for navigation and actions
- 📁 Directory management: Create, rename, and delete directories on the fly

## 🖼️ Screenshots

<div align="center">
  <img src="assets/screenshot1.png" alt="BFileX Main Interface" width="600"/>
  <img src="assets/screenshot2.png" alt="BFileX Main Interface" width="600"/>
  <img src="assets/screenshot3.png" alt="BFileX Main Interface" width="600"/>
  <br/>
  <em>BFileX running in terminal with color-coded files and icons</em>
</div>

## 🚀 Installation

### Prerequisites

- C++17 or higher
    - CMake 3.15 or higher
    - Git

### Build from Source

#### Clone the repository

```bash

git clone https://github.com/BahaaMohamed98/BfileX.git
cd BfileX
```

#### Initialize submodules

```bash
git submodule update --init --recursive
```

#### Build using Cmake

```bash
cd build
cmake ..
cmake --build .
```

#### Run the app

```bash
./BFileX
```

## 🎮 Default Keybindings

| Key                                                   | Action                        |
|-------------------------------------------------------|-------------------------------|
| <kbd>j</kbd>, <kbd>Tab</kbd>, <kbd>Right Arrow</kbd>  | Move down                     |
| <kbd>k</kbd>, <kbd>Up Arrow</kbd>                     | Move up                       |
| <kbd>l</kbd>, <kbd>Enter</kbd>,<kbd>Right Arrow</kbd> | Open file/directory           |
| <kbd>h</kbd>, <kbd>Left arrow</kbd>                   | Go to parent directory        |
| <kbd>r</kbd>                                          | Rename current entry          |
| <kbd>d</kbd>                                          | Delete current entry          |
| <kbd>p</kbd>                                          | Toggle preview                |
| <kbd>t</kbd>                                          | Toggle sorting by time        |
| <kbd>m</kbd>                                          | Make a new directory          |
| <kbd>H</kbd>                                          | Toggle showing hidden entries |
| <kbd>q</kbd>                                          | Quit                          |

## 📄 License

This project is licensed under the Apache 2.0 License - see the [LICENSE](LICENSE) file for details.