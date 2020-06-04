#include "rasterization.h"
#include "math.h"

std::vector<Pixel> simple(Pixel p0, Pixel p1){
	std::vector<Pixel> out;
	int x0 = p0.x, y0 = p0.y;
	int x1 = p1.x, y1 = p1.y;

	float m = float(y1 - y0)/(x1 - x0);
	float b = y0 - m*x0;
	for(int x=x0; x <= x1; x++){
		int y = round(m*x + b);
		out.push_back({x, y});
	}
	return out;
}

std::vector<Pixel> dda(Pixel p0, Pixel p1){
	std::vector<Pixel> out;
	int x0 = p0.x, y0 = p0.y;
	int x1 = p1.x, y1 = p1.y;

	float dx = (x1-x0);
	float dy = (y1-y0);

	float delta = std::max(abs(dx), abs(dy));

	dx /= delta;
	dy /= delta;

	float x = x0;
	float y = y0;

	for(int i = 0; i <= delta; i++){
		out.push_back({(int)round(x), (int)round(y)});
		x += dx;
		y += dy;
	}
	return out;
}

std::vector<Pixel> bres(Pixel P){
	int dx = P.x;
	int dy = P.y;

	std::vector<Pixel> out;

	if(dy > dx){
		out = bres({dy, dx});
		for(Pixel& p: out)
			std::swap(p.x, p.y);
		return out;
	}

	out.reserve(dx);

	int D = 2*dy - dx;
	int y = 0;
	for(int x = 0; x <= dx; x++){
		out.push_back({x, y});
		if(D > 0){
			D -= 2*dx;
			y++;
		}
		D += 2*dy;
	}
	return out;
}

std::vector<Pixel> bresenham(Pixel p0, Pixel p1){
	int x0 = p0.x, y0 = p0.y;
	int x1 = p1.x, y1 = p1.y;

	if(x0 > x1){
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	std::vector<Pixel> out = bres({dx, abs(dy)});

	if(dy >= 0)
		for(Pixel& p: out)
			p = {x0 + p.x, y0 + p.y};
	else
		for(Pixel& p: out)
			p = {x0 + p.x, y0 - p.y};

	return out;
}

std::vector<Pixel> rasterizeCircle(Pixel C, int R){
	std::vector<Pixel> out;
    int y = R;
	int d = 1-R;
	for(int x = 0; x<=y; ++x){
		out.push_back({C.x + x, C.y + y});
		out.push_back({C.x + x, C.y - y});
		out.push_back({C.x - x, C.y - y});
		out.push_back({C.x - x, C.y + y});
		out.push_back({C.x + y, C.y + x});
		out.push_back({C.x + y, C.y - x});
		out.push_back({C.x - y, C.y - x});
		out.push_back({C.x - y, C.y + x});


        if(d > 0){
			d += 2*x-2*y+5;
			y--;
		}else
			d += 2*x + 3;
	}
	return out;

	return out;
}

std::vector<Pixel> rasterizeTriangle(std::array<vec2, 3> P){
	std::vector<Pixel> out;
	std::vector<float> pontos;

    int maxy = floor(std::max(P[0].y, std::max(P[1].y,P[2].y)));
	int miny = ceil(std::min(P[0].y, std::min(P[1].y,P[2].y)));

	double deltax1 = (P[1].x - P[0].x);
	double m1 = (P[1].y - P[0].y)/deltax1;
	double b1 = P[0].y - m1*(P[0].x);

	double deltax2 = (P[2].x - P[1].x);
	double m2 = (P[2].y - P[1].y)/deltax2;
	double b2 = P[2].y - m2*(P[2].x);

	double deltax3 = (P[0].x - P[2].x);
	double m3 = (P[0].y - P[2].y)/deltax3;
	double b3 = P[2].y - m3*(P[2].x);


	for(int y=miny; y <= maxy; y++){

        pontos.clear();

        /*if(deltax1==0){
            if(std::min(P[0].y,P[1].y) <= y && y <= std::max(P[0].y,P[1].y))
              pontos.push_back(P[0].x);
        }

        if(deltax2==0){
            if(std::min(P[1].y,P[2].y) <= y && y <= std::max(P[1].y,P[2].y))
              pontos.push_back(P[1].x);
        }

        if(deltax3==0){
            if(std::min(P[0].y,P[2].y) <= y && y <= std::max(P[0].y,P[2].y))
              pontos.push_back(P[0].x);
        }*/

        if(deltax1==0){
            if((P[0].y <= y && y<= P[1].y) || (P[1].y <= y && y<= P[0].y))
              pontos.push_back(P[0].x);
        }

        if(deltax2==0){
            if((P[1].y  <= y && y<= P[2].y) || (P[2].y <= y && y<= P[1].y))
              pontos.push_back(P[1].x);
        }

        if(deltax3==0){
            if((P[0].y <= y && y<= P[2].y) || (P[2].y <= y && y<= P[0].y))
              pontos.push_back(P[0].x);
        }

        float v1 = (float) (y - b1)/m1;
        float v2 = (float) (y - b2)/m2;
        float v3 = (float) (y - b3)/m3;


        if(std::min(P[0].x,P[1].x) <= v1 && v1 <= std::max(P[0].x,P[1].x))
            pontos.push_back(int(v1));

        if(std::min(P[1].x,P[2].x) <= v2 && v2 <= std::max(P[1].x,P[2].x))
            pontos.push_back(int(v2));

        if(std::min(P[0].x,P[2].x) <= v3 && v3 <= std::max(P[0].x,P[2].x))
            pontos.push_back(int(v3));

        if(pontos.size()==2){
            int maxsx = floor(std::max(pontos[0], pontos[1]));
            int minsx = ceil(std::min(pontos[0], pontos[1]));

            for(int x=minsx; x<= maxsx; x++)
                out.push_back({x,y});
        }


        else if(pontos.size()==3){
            int maxsx = floor(std::max(pontos[0], std::max(pontos[1], pontos[2])));
            int minsx = ceil(std::min(pontos[0], std::min(pontos[1], pontos[2])));

            for(int x=minsx; x<= maxsx; x++)
                out.push_back({x,y});


        }
    }


	return out;
}
