# BFileX

<div align="center">
  <img src="assets/logo.svg" alt="Terminal++ Logo" width="600" style="margin: 20px 0"/>
</div>
<br>

A modern, lightweight terminal file explorer written in C++. BFileX provides an intuitive interface for navigating and
managing your files with vim-style keybindings and a color-coded display.

## ✨ Features

- 🎨 Color-coded interface for different file types
- 📁 Vim-style navigation
- 🚀 Fast and lightweight
- 🔍 File type detection with custom icons
- 📊 Detailed file information (permissions, size, modified date)
- 🖥️ Cross-platform support (Linux, macOS, Windows)
- 🔄 Real-time updates for file system changes
- ⬆️ Comprehensive keybindings for navigation and actions

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
make
```

#### Run the app

```bash
./BFileX
```

## 🎮 Default Keybindings

| Key                        | Action                    |
|----------------------------|---------------------------|
| `j`, `Tab`, `Right Arrow`  | Move down                 |
| `k`, `Up Arrow`            | Move up                   |
| `l`, `Enter`,`Right Arrow` | Open file/Enter directory |
| `h`, `Left arrow`          | Go to parent directory    |
| `r`                        | Rename file/directory     |
| `d`                        | Delete file/directory     |
| `p`                        | Toggle preview            |
| `t`                        | Toggle sorting by time    |
| `q`, `Esc`                 | Quit                      |
