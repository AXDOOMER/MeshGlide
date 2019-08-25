// Copyright (C) 2019 Alexandre-Xavier Labonté-Lamoureux
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
// main.cpp
// Call the function that contains the main loop. Catch any exceptions.

#include "mainloop.h"

#include <SDL2/SDL.h>		/* SDL_ShowSimpleMessageBox */

#include <iostream>
#include <stdexcept>
#include <cstdlib>		/* EXIT_FAILURE, EXIT_SUCCESS */

using namespace std;

int main(int argc, const char* argv[])
{
//	try
	{
		return mainloop(argc, argv);
	}
/*	catch (const exception& e)
	{
		cerr << "Fatal error: " << e.what() << endl;

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Fatal error",
			e.what(),
			NULL);

		return EXIT_FAILURE;
	}*/
}
