// Copyright (C) 2014-2017 Alexandre-Xavier Labonté-Lamoureux
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// textures.cpp
// Texture loader

#include <string>
#include <iostream>
#include <stdexcept>
#include <utility>	// swap
using namespace std;

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include "texture.h"

//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/#compressed-textures

Texture::Texture(const string& Path)
{
	// Surface: Blue, Green, Red
	SDL_Surface* Image = SDL_LoadBMP(Path.c_str());

	if (!Image)
	{
		throw runtime_error("Error loading texture: " + Path);
	}

	_Name = Path;
	_Width = Image->w;
	_Height = Image->h;

	// Create an OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// Bind the texture so that the next functions will modify that texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Protect the following code from a segmentation fault if the picture
	// doesn't contain the expected number of bits per pixel.
	// Improvement: SDL_ConvertSurfaceFormat ?
	if (Image->format->BitsPerPixel != 24)
	{
		throw runtime_error("Texture " + Path + " is not 24 bits per pixel!");
	}

	// Flip the buffer.
	Uint8 *pixels = (Uint8 *)Image->pixels;
	unsigned int bytes = Image->w * Image->h * 3;
	for (int i = 0; i < bytes / 2; i++)
	{
		swap(pixels[i], pixels[bytes - i - 1]);
	}

	// Load the texture. The GL_BGR format should be used, but the bytes of the buffer have been flipped above.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Image->w, Image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, Image->pixels);

	// Repeat texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum ErrorCode = glGetError();
	if (ErrorCode != GL_NO_ERROR)
	{
		throw runtime_error((const char*)gluErrorString(ErrorCode));
	}

	// Set the ID and free the surface
	_Id = textureID;
	SDL_FreeSurface(Image);

	cout << "Texture loaded: '" << Path << "' is " << Image->w << "x" << Image->h << endl;	// Debug
}

string Texture::Name() const
{
	return _Name;
}

unsigned int Texture::Id() const
{
	return _Id;
}

unsigned short Texture::Width() const
{
	return _Width;
}

unsigned short Texture::Height() const
{
	return _Height;
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, _Id);
}

Texture::~Texture() {
	cout << "Deleting texture " << _Name << " (" << _Id << ")" << endl;
	glDeleteTextures(1, &_Id);
}
