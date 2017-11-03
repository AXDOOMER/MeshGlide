# FPS_test (previously a prototype for [KillBox](https://github.com/AXDOOMER/KillBox))

A simple FPS game that uses GLFW for the graphics and input.
It is distributed under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html).

This was a prototype to test the feasibility of a homemade OpenGL engine for [KillBox](https://github.com/AXDOOMER/KillBox).

### Dev

Compile and run on Linux: `g++ *.cpp -std=c++11 -lglfw -lGL -lGLU -lglut -lSDL -o FPS_test && ./FPS_test`

Packages that may need to be installed (Debian): `sudo apt-get install libglfw3 libglfw3-dev`

