
#ifndef GRAPHICS2D_H
#define GRAPHICS2D_H

#include <vector>
#include "Image.h"
#include "Primitives.h"
#include "rasterization.h"

class Graphics2D{
	Image img;

	public:
	Graphics2D(int w, int h) : img{w, h}{}

	void clear(Color col = white){
		img.fill(col);
	}

	void savePNG(const char* filename){
		img.savePNG(filename);
	}

	template<class Prims>
	void draw(
		const std::vector<vec2>& V,
		const Prims& P,
		Color color
	){
		for(size_t i = 0; i < P.size(); i++)
			draw_primitive(assemble(V,P[i]), color);
	}

	private:

	void draw_primitive(Line<vec2> line, Color color){
		Pixel p0 = {(int)round(line[0].x), (int)round(line[0].y)};
		Pixel p1 = {(int)round(line[1].x), (int)round(line[1].y)};

		for(Pixel p: bresenham(p0, p1)){
			if(p.x >= 0 && p.y >= 0 && p.x < img.width && p.y < img.height)
			img(p.x, p.y) = color;
		}

	}

	void draw_primitive(Triangle<vec2> tri, Color color){
        for(Pixel p : rasterizeTriangle(tri))
            img(p.x, p.y) = color;
    }
};

#endif
