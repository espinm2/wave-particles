#ifndef UTILITES_H
#define UTILITES_H

#include <iostream>
#include <vector>
#include "vectors.h"

/*
 * This file contains general algorithms used in our program.
 * The code in here is independent of our simulation.
 */


// This function returns vertex of points of locations to "plot"
// The output is in order { x1, y1, x2, y2, .... xn, yn}
// This is a interger arthmetic version of the program credit wiki
std::vector<int> bresenham_line_plot(const int& _x0, const int& _y0,
                                     const int& _x1, const int& _y1);

// Given an x and y coodinate, get the octant I am located in
// This is value from [0 to 7]
int getOctantOfLine(const int& x0, const int& y0,
                                     const int& x1, const int& y1);


// This will switch the octant of a point by referance
void switchToOctantZeroFrom(int octant, const int & _x, const int & _y, int & x, int & y);
void switchToOctantFromZero(int octant, const int & _x, const int & _y, int & x, int & y);

#endif // UTILITES_H

