#ifndef GRAPHICS3D_H
#define GRAPHICS3D_H

#include <vector>
#include "geometry.h"
#include "Image.h"
#include "Primitives.h"
#include "rasterization.h"
#include "matrix.h"
#include "SimpleShader.h"

class Graphics3D{
	Image img;
	std::vector<float> ZBuffer;

	public:
	Graphics3D(int w, int h) : img{w, h}{}

	void clear(Color col = black){
		img.fill(col);
	}

	void savePNG(const char* filename){
		img.savePNG(filename);
	}

	void initZBuffer(){
		ZBuffer.assign(img.pixels.size(), -1.0f);
	}

	float& zbuffer(int x, int y){
		return ZBuffer[y*img.width + x];
	}

	bool ZBufferTest(int x, int y, float z){
		if(ZBuffer.empty())
			return true;

		float& zb = zbuffer(x, y);
		if(z > zb){
			zb = z;
			return true;
		}

		return false;
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

        float z = v.position.z/v.position.w;

		if(ZBufferTest(p.x, p.y, z))
            fragmentShader(v, img(p.x,p.y));
	}

	template<class Varying>
	void drawPrimitive(Line<Varying> line){
	    if(clip(line))
            return;

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
    bool clip(Line<Varying>& line){

        vec4 n[] = {
            {0, 0, -1, 1},
            {0, 0, 1, 1},
            {1, 0, 0, .9},
            {-1, 0, 0, .9},
            {0, 1, 0, .9},
            {0, -1, 0, .9},
        };


          for(int i=0; i<6; i++){

              float pn = dot(line[0].position,n[i]);
              float qn = dot(line[1].position,n[i]);

              if (pn < 0 && qn < 0)
                return true;
              else if((pn>0 && qn<0)){
                float t = pn/(pn-qn);
                vec4 c= lerp(t,line[0].position,line[1].position);
                line[1].position = c;
              }
              else if (pn<0 && qn>0){
                float t = pn/(pn-qn);
                vec4 c = lerp(t,line[0].position,line[1].position);
                line[0].position = c;
              }

              }
              return false;

    }

	template<class Varying>
	void drawPrimitive(Triangle<Varying> tri){
		if(clip(tri))
			return;

		vec2 v0 = toScreen(tri[0].position),
             v1 = toScreen(tri[1].position),
             v2 = toScreen(tri[2].position);

		std::array<vec2,3> pontos={v0,v1,v2};

        for(Pixel p : rasterizeTriangle(pontos)){
           vec2 v = {(float)p.x, (float)p.y};
           std::array<float, 3> b = barycentric(v,pontos);
           Varying vi = ((b[0]*tri[0])+ (b[1]*tri[1]) + (b[2]*tri[2]));
           paint(p, vi);
        }
	}

	template<class Varying>
	bool clip(Triangle<Varying>& tri){
		vec4 n[] = {
			{0, 0, -1, 1},
			{0, 0, 1, 1},
			{1, 0, 0, 1},
			{-1, 0, 0, 1},
			{0, 1, 0, 1},
			{0, -1, 0, 1},
		};
		for(int i = 0; i < 6; i++)
			if(clip(tri, n[i]))
				return true;
		return false;
	}

	template<class Varying>
	bool clip(Triangle<Varying>& tri, vec4 n){
	    float pn = dot(tri[0].position,n);
        float qn = dot(tri[1].position,n);
        float rn = dot(tri[2].position,n);

        if(pn<0 && qn< 0 && rn<0)
            return true;
        else if(pn>0 && qn>0 && rn>0)
            return false;

        std::vector<Varying> L;

        int j =1;

        for(int i=0; i<3; i++, j++){
                if(i==2)
                   j=0;

            if((dot(tri[i].position,n)<0 && dot(tri[j].position,n)>=0) || (dot(tri[i].position,n)>= 0 && dot(tri[j].position,n)<0)){
                    float t = dot(tri[i].position, n)/(dot(tri[i].position,n)-dot(tri[j].position,n));
                    Varying aux1;
                    aux1=(1-t)*tri[i] + t*tri[j];
                    L.push_back(aux1);
                }

                if(dot(tri[j].position,n)>=0)
                    L.push_back(tri[j]);


            }

                tri = {L[0], L[1], L[2]};
                if(L.size()==4)
                {
                    Triangle<Varying> ntri = {L[0], L[2], L[3]};
                    drawPrimitive(ntri);
                }

        return false;


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
    double theta = alpha*M_PI/180.0;
    float t = fabs(n) * tan((theta)/2), b = -t;
    float r = t*a, l = -r;

    return frustum(l,r,b,t,-n,-f);
}


inline mat4 lookAt(vec3 o, vec3 c, vec3 up){
    vec3 up1 = normalize(up);
    vec3 f = normalize((c-o));
    vec3 s = normalize(cross(f,up1));
    vec3 u = cross(s,f);

    mat4 mv_1 = {
         s.x,  s.y,  s.z, 0.0,
         u.x,  u.y,  u.z, 0.0,
        -f.x, -f.y, -f.z, 0.0,
         0.0,  0.0,  0.0, 1.0
    };

    mat4 mv_2 = translate(-o.x,-o.y,-o.z);

    return mv_1 * mv_2;

}






#endif

