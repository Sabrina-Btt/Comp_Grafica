
#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include "Color.h"

class Image{
	public:
	int width = 0, height = 0;
	std::vector<Color> pixels;

	Image() = default;

	Image(int w, int h):
		width(w), height(h), pixels(w*h)
	{}

	Image(const char* filename);

	Color& operator()(int x, int y){
		return pixels[y*width + x];
	}

	void fill(Color color){
		pixels.assign(pixels.size(), color);
	}

	void savePNG(const char* filename);
	void saveBMP(const char* filename);
};


#endif
