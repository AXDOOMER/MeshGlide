# FPS_test (previously a prototype for [KillBox](https://github.com/AXDOOMER/KillBox))

A simple FPS game that uses GLFW for the graphics and input. It is distributed under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html).

It was a prototype to test the feasibility of a homemade OpenGL engine for [KillBox](https://github.com/AXDOOMER/KillBox).

It is now under development. The goal is to create an engine better than the [KillBox](https://github.com/AXDOOMER/KillBox) engine.

### Dev

Compile and run on Linux: `g++ *.cpp -std=c++11 -lglfw -lGL -lGLU -lglut -lSDL -o FPS_test && ./FPS_test`

You may need to remove `physics.cpp`. It is preferable to compile and run the program using the `run.sh` script.

Packages that may need to be installed (Debian): `apt-get install libglfw3 libglfw3-dev libsdl2-image-2.0-0 libsdl2-image-dev freeglut3 freeglut3-dev`

### Random screenshots

![screenshot at 2017-12-18 19-50-42](https://user-images.githubusercontent.com/6194072/34324771-efb26d0a-e84b-11e7-9c4a-a0529cafe437.png)
![screenshot at 2017-12-18 19-52-53](https://user-images.githubusercontent.com/6194072/34324772-efc4187a-e84b-11e7-8282-05aad338cde3.png)
![screenshot at 2017-12-18 19-54-07](https://user-images.githubusercontent.com/6194072/34324773-efd47c06-e84b-11e7-8a3c-71d3ca3fc26e.png)
![screenshot-untitled window](https://user-images.githubusercontent.com/6194072/34324774-efe54d1a-e84b-11e7-9193-9fe29b546288.png)
