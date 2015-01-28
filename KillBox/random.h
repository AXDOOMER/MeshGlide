// random.h
//Copyright (C) 2014 Alexandre-Xavier Labonté-Lamoureux
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Pseudo-random number generator (PRNG)

static unsigned short Index = 0;
const int PRA_SIZE = 256;

//Values between 0 and 255 inluded
const unsigned char RandomNumbers[256] = {
	33, 92, 54, 71, 56, 85, 64, 80, 77, 210, 206, 149, 207, 171, 191, 245,
	255, 94, 49, 4, 75, 115, 227, 97, 50, 5, 215, 234, 225, 205, 107, 137,
	160, 254, 9, 51, 69, 244, 160, 48, 66, 24, 93, 179, 207, 185, 131, 102,
	249, 104, 57, 21, 22, 56, 253, 175, 214, 70, 170, 212, 84, 247, 187, 158,
	177, 135, 240, 80, 75, 198, 83, 178, 229, 141, 187, 199, 181, 94, 36, 46,
	61, 156, 202, 242, 168, 49, 34, 24, 92, 187, 173, 156, 243, 68, 136, 158,
	62, 81, 195, 245, 115, 153, 161, 159, 219, 77, 198, 139, 162, 120, 63, 133,
	158, 34, 97, 187, 192, 255, 244, 216, 182, 173, 241, 240, 223, 170, 244, 161,
	160, 225, 237, 169, 152, 198, 227, 140, 170, 164, 199, 145, 65, 235, 144, 205,
	172, 216, 169, 209, 198, 184, 62, 79, 28, 53, 25, 73, 66, 16, 241, 211,
	35, 210, 199, 222, 175, 140, 147, 68, 180, 246, 177, 242, 226, 158, 253, 216,
	204, 217, 84, 236, 163, 172, 166, 127, 252, 175, 99, 93, 68, 55, 69, 93,
	22, 76, 11, 80, 36, 42, 53, 51, 54, 108, 137, 43, 158, 218, 183, 171,
	194, 135, 217, 109, 233, 248, 227, 195, 157, 227, 172, 221, 140, 164, 156, 32,
	35, 75, 141, 249, 48, 58, 126, 37, 142, 240, 98, 39, 13, 32, 189, 56,
	156, 109, 231, 245, 143, 144, 141, 192, 165, 184, 39, 22, 56, 104, 188, 174
};

//Use: Modulo(%), Spacing(*), Modifier(+)
int R_Random(int Modulo, int Spacing, int Modifier);

