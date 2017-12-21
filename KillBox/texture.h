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
// texture.h
// Texture loader

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <GL/gl.h>

#include <string>
using namespace std;

class Texture
{
private:
	string _Name;
	GLuint _Id;
	unsigned short _Width;
	unsigned short _Height;
public:
	Texture() = delete;
	Texture(const string& Path);
	~Texture();

	string Name() const;
	string Extension() const;
	unsigned int Id() const;
	unsigned short Width() const;
	unsigned short Height() const;
	void Bind();
};

#endif
