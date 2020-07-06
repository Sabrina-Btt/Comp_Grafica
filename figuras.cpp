#include "Graphics3D.h"
#include "vec.h"
#include "SimpleShader.h"

int main(){

	vector<vec3> V = {
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
        {0, 1, 0},
        {0, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 1, 1},
    };
    vector<size_t> indices = {
        0, 1,    1, 2,    2, 3,    3, 0,    4, 5,    5, 6,
        6, 7,    7, 4,    0, 4,    1, 5,    2, 6,    3, 7
    };
    Elements<Lines> Le{indices};

    mat4 Model1 = rotate_y(0.2)*rotate_x(0.3)*rotate_z(0.3)*translate(0.3, 0.3, 0.3)*scale(0.45,0.45,0.45);
    mat4 Model2 = rotate_y(-0.25)*rotate_x(0.60)*translate(-0.40, -2.0, -2.0)*scale(0.15,0.3,0.2);
    mat4 Model3 = rotate_y(0.8)*rotate_x(0.3)*translate(-0.4, -0.4, -0.4)*scale(0.15,1,0.3);
    mat4 View = translate(0, 0, -1);
    mat4 Projection = orthogonal(-1, 1, -1, 1, -20, 20);

    Graphics3D G{500, 500};
    G.clear();

    SimpleShader::M = Projection*View*Model1;
    SimpleShader::C = blue;
    G.draw<SimpleShader::Varying>(V, Le);

    SimpleShader::M = Projection*View*Model2;
    SimpleShader::C = green;
    G.draw<SimpleShader::Varying>(V, Le);

    SimpleShader::M = Projection*View*Model3;
    SimpleShader::C = magenta;
    G.draw<SimpleShader::Varying>(V, Le);

    G.savePNG("output.png");

}



