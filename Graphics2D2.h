#ifndef GRAPHICS2D_H
#define GRAPHICS2D_H

#include <vector>
#include "Image.h"
#include "Primitives.h"
#include "rasterization.h"
#include "geometry.h"
#include "vec.h"
#include "Color.h"

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

	void paint(Pixel p, Color c){
		if(p.x >= 0 && p.y >= 0 && p.x < img.width && p.y < img.height)
			img(p.x,p.y) = c;
	}


	template<class Prims>
	void draw(const std::vector<Vec2Col>& V, const Prims& P){
		for(size_t i = 0; i < P.size(); i++)
			draw_primitive(assemble(V, P[i]));
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

	void drawPrimitive(Line<Vec2Col> line){
		vec2 v0 = line[0].position, v1 = line[1].position;
		Color c0 = line[0].color, c1 = line[1].color;

		Pixel p0 = {(int)round(v0.x), (int)round(v0.y)};
		Pixel p1 = {(int)round(v1.x), (int)round(v1.y)};

		for(Pixel p: bresenham(p0, p1)){
			vec2 v = {(float)p.x, (float)p.y};
			float t = findMixParam(v, v0, v1);
			Color color = lerp(t, c0, c1);
			paint(p, color);
		};
	}


	void draw_primitive(Triangle<Vec2Col> tri){
		vec2 v0 = tri[0].position, v1 = tri[1].position, v2 = tri[2].position;
		Color c0 = tri[0].color, c1 = tri[1].color, c2 = tri[2].color;

		std::array<vec2,3> pontos={v0,v1,v2};

        for(Pixel p : rasterizeTriangle(pontos)){
           vec2 v = {(float)p.x, (float)p.y};
           std::array<float, 3> b = barycentric(v,pontos);
           Color color = toColor((b[0]*toVec3(c0))+ (b[1]*toVec3(c1)) + (b[2]*toVec3(c2)));
           paint(p, color);
        }

    }

};



#endif
