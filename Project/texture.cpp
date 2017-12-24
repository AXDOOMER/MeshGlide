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
// texture.cpp
// Texture loader

#include "texture.h"

#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <string>
#include <iostream>
#include <stdexcept>
#include <utility>	/* swap */
using namespace std;

Texture::Texture(const string& Path)
{
	// Surface: Blue, Green, Red
	SDL_Surface* Surface = IMG_Load(Path.c_str());

	if (!Surface)
	{
		throw runtime_error("Error loading texture: " + Path);
	}

	_Name = Path;
	_Width = Surface->w;
	_Height = Surface->h;

	// Create an OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// Bind the texture so that the next functions will modify that texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Don't support other file extensions because they were not tested
	if (Extension() != "jpg" && Extension() != "png")
	{
		throw runtime_error("File " + Path + " has unsupported extension.");
	}

	GLint Mode;

	string bits = to_string(Surface->format->BitsPerPixel);

	if (Surface->format->BitsPerPixel == 24)
	{
		Mode = GL_RGB;

		// Flip the buffer by 180 degrees.
		Uint8* pixels = (Uint8*)Surface->pixels;
		unsigned int bytes = Surface->w * Surface->h * 3;
		for (int i = 0; i < bytes / 2; i++)
		{
			swap(pixels[i], pixels[bytes - i - 1]);
		}
		// Because of the flip, colors are now in BGR format. Swap blue and red bytes to become RGB.
		for (int i = 0; i < bytes; i += 3)
		{
			swap(pixels[i], pixels[i + 2]);
		}
	}
	else if (Surface->format->BitsPerPixel == 32)
	{
		Mode = GL_RGBA;

		// Flip the buffer by 180 degrees.
		Uint32* pixels = (Uint32*)Surface->pixels;
		unsigned int bytes = Surface->w * Surface->h;
		for (int i = 0; i < bytes / 2; i++)
		{
			swap(pixels[i], pixels[bytes - i - 1]);
		}
	}
	else
	{
		throw runtime_error("Texture " + Path + " has an unsupported number of bits per pixel: " + bits);
	}

	// Load the texture. The GL_BGR format should be used, but the bytes of the buffer have been flipped above.
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);

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
	SDL_FreeSurface(Surface);

	cout << "Texture loaded: '" << Path << "' is " << Surface->w << 'x' << Surface->h << 'x' << bits << endl;
}

string Texture::Name() const
{
	return _Name;
}

string Texture::Extension() const
{
	if (_Name.find_last_of(".") != string::npos)
		return _Name.substr(_Name.find_last_of(".") + 1);
	return "";
}

GLuint Texture::Id() const
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
