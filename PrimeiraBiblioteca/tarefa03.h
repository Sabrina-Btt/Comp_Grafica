#include "Image.h"
#include "rasterization.h"
#include "Color.h"

void geraCirculo(){
    Image Img(600, 600);
	Img.fill(white);

	int n = 20;
	for(int i = 0; i < n; i++){
		float angle = i*2*M_PI/n;
		int x0 = round(300 +  50*cos(angle));
		int y0 = round(300 +  50*sin(angle));
		int x1 = round(300 + 200*cos(angle));
		int y1 = round(300 + 200*sin(angle));
		for(Pixel p : bresenham({x0, y0}, {x1, y1}))
			Img(p.x, p.y) = blue;
	}
	Img.savePNG("output.png");
}

Image draw_circle(Image Img, int cx, int cy, int R, Color color){
    std::vector<Pixel> pintar;
    pintar = rasterizeCircle({cx, cy},R);
    for(Pixel p: pintar)
        Img(p.x, p.y) = color;
    return Img;
}
