// Copyright (C) 2015 Jeff Thompson
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
// line.h
// Line/Circle collision code
// Source: https://github.com/jeffThompson/CollisionDetection/blob/master/CodeExamples/LineCircle/LineCircle.pde

#include <cmath>

float dist(float x1, float y1, float x2, float y2)
{
	float distX = x1 - x2;
	float distY = y1 - y2;

	return sqrt((distX * distX) + (distY * distY));
}

// POINT/CIRCLE
bool pointCircle(float px, float py, float cx, float cy, float r) {

	// get distance between the point and circle's center
	// using the Pythagorean Theorem
	float distX = px - cx;
	float distY = py - cy;
	float distance = sqrt((distX * distX) + (distY * distY));

	// if the distance is less than the circle's
	// radius the point is inside!
	if (distance <= r) {
		return true;
	}
	return false;
}

// LINE/POINT
bool linePoint(float x1, float y1, float x2, float y2, float px, float py) {

	// get distance from the point to the two ends of the line
	float d1 = dist(px, py, x1, y1);
	float d2 = dist(px, py, x2, y2);

	// get the length of the line
	float lineLen = dist(x1, y1, x2, y2);

	// since floats are so minutely accurate, add
	// a little buffer zone that will give collision
	float buffer = 0.1;    // higher # = less accurate

	// if the two distances are equal to the line's
	// length, the point is on the line!
	// note we use the buffer here to give a range, rather
	// than one #
	if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer) {
		return true;
	}
	return false;
}


// LINE/CIRCLE
bool lineCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r) {

	// is either end INSIDE the circle?
	// if so, return true immediately
	bool inside1 = pointCircle(x1, y1, cx, cy, r);
	bool inside2 = pointCircle(x2, y2, cx, cy, r);
	if (inside1 || inside2)
		return true;

	// get length of the line
	float distX = x1 - x2;
	float distY = y1 - y2;
	float len = sqrt((distX * distX) + (distY * distY));

	// get dot product of the line and circle
	float dot = (((cx - x1) * (x2 - x1)) + ((cy - y1) * (y2 - y1))) / pow(len, 2);

	// find the closest point on the line
	float closestX = x1 + (dot * (x2 - x1));
	float closestY = y1 + (dot * (y2 - y1));

	// is this point actually on the line segment?
	// if so keep going, but if not, return false
	bool onSegment = linePoint(x1, y1, x2, y2, closestX, closestY);
	if (!onSegment)
		return false;

	// get distance to closest point
	distX = closestX - cx;
	distY = closestY - cy;
	float distance = sqrt((distX * distX) + (distY * distY));

	// is the circle on the line?
	if (distance <= r) {
		return true;
	}
	return false;
}
