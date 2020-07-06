#ifndef GRAPHICS3D_H
#define GRAPHICS3D_H

#include <vector>
#include "geometry.h"
#include "Image.h"
#include "Primitives.h"
#include "rasterization.h"
#include "matrix.h"

class Graphics3D{
	Image img;

	public:
	Graphics3D(int w, int h) : img{w, h}{}

	void clear(Color col = white){
		img.fill(col);
	}

	void savePNG(const char* filename){
		img.savePNG(filename);
	}

	template<class Varying, class Vertex, class Prims>
	void draw(const std::vector<Vertex>& V, const Prims& p){
		std::vector<Varying> PV(V.size());
		for(size_t i = 0; i < V.size(); i++)
			vertexShader(V[i], PV[i]);

		for(size_t i = 0; i < p.size(); i++)
			drawPrimitive(assemble(PV, p[i]));
	}

	private:
	vec2 toScreen(vec4 P){
		return {
			((P.x/P.w + 1)*img.width - 1)/2,
			((P.y/P.w + 1)*img.height - 1)/2
		};
	}

	template<class Varying>
	void paint(Pixel p, Varying v){
		if(p.x < 0 || p.y < 0 || p.x >= img.width || p.y >= img.height)
			return;

		fragmentShader(v, img(p.x,p.y));
	}

	template<class Varying>
	void drawPrimitive(Line<Varying> line){
		vec2 v0 = toScreen(line[0].position);
		vec2 v1 = toScreen(line[1].position);

		Pixel p0 = {(int)round(v0.x), (int)round(v0.y)};
		Pixel p1 = {(int)round(v1.x), (int)round(v1.y)};

		for(Pixel p: bresenham(p0, p1)){
			vec2 v = {(float)p.x, (float)p.y};
			float t = findMixParam(v, v0, v1);
			Varying vi = (1-t)*line[0] + t*line[1];
			paint(p, vi);
		};
	}

	template<class Varying>
	void drawPrimitive(Triangle<Varying> tri){
		// TAREFA
	}


};

//Projection Matrix

inline mat4 orthogonal(float l, float r, float b, float t, float n, float f){
    return{
         2/(r-l),   0.0,     0.0,    ((r+l)/(r-l))*(-1),
           0.0,   2/(t-b),   0.0,    ((t+b)/(t-b))*(-1),
           0.0,     0.0,   2/(n-f),  ((n+f)/(n-f))*(-1),
           0.0,     0.0,     0.0,            1
    };
}


inline mat4 frustum(float l, float r, float b, float t, float n, float f){
    return{
         -2*n/(r-l),   0.0,      (r+l)/(r-l),      0.0  ,
           0.0,     -2*n/(t-b),  (t+b)/(t-b),      0.0  ,
           0.0,        0.0,      (f+n)/(f-n),  2*n*f/(n-f),
           0.0,        0.0,          -1,           0.0
    };
}

inline mat4 perspective(float alpha, float a, float n, float f){
    float t = fabs(n) * tan((alpha)/2), b = -t;
    float r = t*a, l = -r;

    return frustum(l,r,b,t,-n,-f);
}


#endif
