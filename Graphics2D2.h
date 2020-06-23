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




	template<class Prims>
	void draw(const std::vector<Vec2Col>& V, const Prims& P){
		for(size_t i = 0; i < P.size(); i++)
			drawPrimitive(assemble(V, P[i]));
	}


	template<class Prims>
	void draw(
		const std::vector<vec2>& V,
		const Prims& P,
		Color color
	){
		for(size_t i = 0; i < P.size(); i++)
			drawPrimitive(assemble(V,P[i]), color);
	}

	private:

    void paint(Pixel p, Color c){
		if(p.x >= 0 && p.y >= 0 && p.x < img.width && p.y < img.height)
			img(p.x,p.y) = c;
	}


	void drawPrimitive(Line<Vec2Col> line){
		if(clip(line))
            return;

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

    bool clip(Line<Vec2Col>& line){
        int N=4;
        float xmin = 30, xmax = (img.width)-30;
        float ymin = 30, ymax = (img.height)-30;

        std::vector<float> p;
        std::vector<float> q;

        float  t_in = 0;
        float  t_out = 1;

        vec2 a,b;

        a = line[0].position;
        b = line[1].position;
        Color ca=line[0].color;
        Color cb=line[1].color;

        p.push_back ((b.x - a.x)*(-1));
        p.push_back (b.x - a.x);
        p.push_back ((b.y - a.y)*(-1));
        p.push_back (b.y - a.y);

        q.push_back (a.x - xmin);
        q.push_back (xmax - a.x);
        q.push_back (a.y - ymin);
        q.push_back (ymax - a.y);



        for(int i=0; i<N;i++){
            if(p[i]<0)
                t_in = std::max(t_in, (q[i]/p[i]));
            else if(p[i] > 0)
                t_out = std::min(t_out, (q[i]/p[i]));
            else if(q[i] < 0)
                return true;

            }


        if(t_in > t_out)
           return true;

        else{
             line[0].position = lerp(t_in,a,b);
             line[0].color=lerp(t_in,ca,cb);
             line[1].position = lerp(t_out,a,b);
             line[1].color=lerp(t_out,ca,cb);

        }
        return false;
    }


	void drawPrimitive(Triangle<Vec2Col> tri){
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
