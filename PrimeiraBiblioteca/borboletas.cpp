#include "Graphics2D.h"
#include "bezier.h"
#include "matrix.h"

 mat3 calculaR(int i, float t){

       mat3 R = {
            cos(t*i), -sin(t*i), 0.0,
            sin(t*i),  cos(t*i), 0.0,
              0.0,       0.0   , 1.0
        };
        return R;
}

int main(){

    int N = 15; // numero de repeticoes

    double t = (2*M_PI/N)/0.5;

    vector<vec2> P = loadCurve("borboleta.txt");
    vector<vec2> Q = sample_cubic_bezier_spline(P, 30);

	vec2 v = {500, 500};
	mat3 T = {
		1.0, 0.0, v.x,
		0.0, 1.0, v.y,
		0.0, 0.0, 1.0
	};

	mat3 Ti = {
		1.0, 0.0, -v.x,
		0.0, 1.0, -v.y,
		0.0, 0.0, 1.0
	};

	mat3 M = {
		1.0, 0.0, 100,
		0.0, 1.0, 100,       //Matriz de translação para encaixar na imagem
		0.0, 0.0, 1.0
	};


    LineStrip L{Q.size()};

	Graphics2D G(1000, 1000);
	G.clear();
	for(int i=0; i<N; i++){
        mat3 R;
        R = calculaR(i,t);
        G.draw((T*(R*M)*Ti)*Q, L, blue);
	}
	G.savePNG("borboletas.png");
}
