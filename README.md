# MeshGlide

![Top Language](https://img.shields.io/github/languages/top/axdoomer/meshglide.svg?style=flat)
![Code Size](https://img.shields.io/github/languages/code-size/axdoomer/meshglide.svg?style=flat)
![License](https://img.shields.io/github/license/axdoomer/meshglide.svg?style=flat&logo=gnu)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/c33bd70b79904cbe8fec5f1cae998b3b)](https://app.codacy.com/app/alexandrexavier/MeshGlide?utm_source=github.com&utm_medium=referral&utm_content=AXDOOMER/MeshGlide&utm_campaign=Badge_Grade_Dashboard)

A simple FPS engine that uses GLFW 3 for the graphics and input. It's distributed under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html).

The early version of the engine was used as a prototype to test the feasibility of a homemade OpenGL engine for [KillBox](https://github.com/AXDOOMER/KillBox).

It's now under development. The goal is to create a new engine that's better than the [KillBox](https://github.com/AXDOOMER/KillBox) engine.

It's called the MeshGlide engine. It's due to the way it's optimized. The player "glides" from polygon to polygon. The data structures are simple and the game runs very fast due to the tiny amount of calculations that have to be made in order to keep track of the player.

## Development

### Installation

Packages that need to be installed under **Debian** and **Ubuntu** are: `libglfw3-dev libsdl2-image-dev libsdl2-dev libczmq-dev`.

Under **Open Suse 15 Leap**, these packages need to be installed: `libglfw-devel freeglut-devel libSDL2_image-devel cppzmq-devel`.

### Compile

Compile on Linux: `g++ *.cpp -std=c++14 -lglfw -lGL -lGLU -lSDL2 -lSDL2_image -lzmq -o MeshGlide`

It's preferable to compile and run the program using the `run.sh` script because it's tested, but this should work too.

### Creating new levels

MeshGlide has a native format, but also supports the OBJ format (with slight modifications). Refer to [this wiki page](https://github.com/AXDOOMER/MeshGlide/wiki/Creating-new-levels) for more details.

## Credits

The following files were taken from the [Freedoom](https://github.com/freedoom/freedoom) project. See their [license](https://github.com/freedoom/freedoom/blob/master/COPYING.adoc).

* Player sprites: playa1.png, playa2.png, playa3.png, playa4.png, playa5.png, playa6.png, playa7.png, playa8.png
* Shotgun sprite: shota0.png
* Rocket launcher sprite: launa0.png
* Minigun sprite: mguna0.png
* Bullet puff: puffa0.png, puffb0.png, puffc0.png, puffd0.png
* Door texture: door9_1.png
* Blood sprites: bluda0.png, bludb0.png, bludc0.png

MeshGlide uses code from [CollisionDetection](https://github.com/jeffThompson/CollisionDetection) written by Jeff Thompson. The code is used in `line.h`. Permission has been granted to use and redistribute it under the GNU GPLv3 which is the same license as MeshGlide. The original license for this code was [CC BY-NC-SA 3.0](https://creativecommons.org/licenses/by-nc-sa/3.0/).

## Screenshots

### Version 0.47

![screenshot_2018-09-02_22-38-51](https://user-images.githubusercontent.com/6194072/44964940-13fd5e00-af01-11e8-92a3-ee4c36e36aa8.png)

### Version 0.39

![screenshot at 2018-05-10 21-06-32](https://user-images.githubusercontent.com/6194072/40698505-4aac5550-639d-11e8-949c-be9c2fab0723.png)

### From an older version

![screenshot at 2017-12-18 19-50-42](https://user-images.githubusercontent.com/6194072/34324771-efb26d0a-e84b-11e7-9c4a-a0529cafe437.png)
