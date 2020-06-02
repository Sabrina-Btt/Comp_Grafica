#ifndef RASTERIZATION_H
#define RASTERIZATION_H

#include "vec.h"
#include <array>

struct Pixel{
	int x, y;
};

std::vector<Pixel> simple(Pixel p0, Pixel p1);

std::vector<Pixel> dda(Pixel p0, Pixel p1);

std::vector<Pixel> bresenham(Pixel p0, Pixel p1);

std::vector<Pixel> rasterizeTriangle(std::array<vec2, 3> P);

std::vector<Pixel> rasterizeCircle(Pixel C, int R);
#endif
