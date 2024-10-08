#!/bin/bash

# Compile the program
g++ main.cpp -o main \
-I/opt/homebrew/Cellar/glew/2.2.0_1/include \
-I/opt/homebrew/Cellar/glfw/3.4/include \
-I/opt/homebrew/include \
-L/opt/homebrew/Cellar/glew/2.2.0_1/lib \
-L/opt/homebrew/Cellar/glfw/3.4/lib \
-L/opt/homebrew/lib \
-lglfw -lGLEW -lglut -framework OpenGL

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the program..."
    ./main
else
    echo "Compilation failed. Please check the errors above."
    # exit 1
fi