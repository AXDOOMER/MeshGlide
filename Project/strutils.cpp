// Copyright (C) 2018 Alexandre-Xavier Labonté-Lamoureux
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
// strutils.cpp
// Functions to help with strings

#include "strutils.h"

#include <string>
#include <vector>
using namespace std;

// Splits a string
// Will not work correctly if multiple delimiters touch each other or if there is a delimiter at the end of the string
vector<string> Split(string s, const char delimiter)
{
	size_t pos = 0;
	vector<string> tokens;
	while ((pos = s.find(delimiter)) != string::npos) {
		tokens.emplace_back(s.substr(0, pos));	// Extract a token and add it to the vector
		s.erase(0, pos + 1);
	}

	// There may not be any delimiter remaining, but let's not forget the last token.
	tokens.push_back(s);

	return tokens;
}

bool EndsWith(const string& str, const string& value)
{
	return str.rfind(value) == str.size() - value.size();
}
