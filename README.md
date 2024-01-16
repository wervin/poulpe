# Poulpe Text Editor

## Description

`Poulpe` is a text editor library for [cimgui](https://github.com/cimgui/cimgui), a thin C wrapper built arround the excellent immediate mode gui [Dear ImGui](https://github.com/ocornut/imgui). `Poulpe` is designed to be lightweight and includes my favorite shortcuts and features from other editors.

## Setup
This project is developed and tested on Ubuntu 23.10. There are no current plans to port it to Windows or Mac.

## Prerequisites
- CMake
- GCC
- LuaJIT

## Building and Running Poulpe Demo

1. **Clone the Repository**

   Clone the `Poulpe` repository to your local machine using Git:
   ```
   git clone https://github.com/wervin/poulpe.git
   cd poulpe
   ```

2. **Configure and Build the Project**

   Create a build directory and navigate into it:
   ```
   mkdir build
   cd build
   ```

   Configure the project build with CMake:
   ```
   cmake ..
   ```

   Compile the project:
   ```
   make
   ```

3. **Running Poulpe Demo**

    After successful compilation, run the `Poulpe` demo:

    ```
    cd demo
    ./demo
    ```

## How to Use it ?

Refer to the `ui.c` source file in the `demo` directory for guidance on how to include and use the `Poulpe` text editor.