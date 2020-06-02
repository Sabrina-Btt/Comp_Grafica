#include "Image.h"

int distRaio(int x1, int y1){
    int dist=sqrt(pow((x1-300),2)+(pow((y1-300),2)));
    return dist;
}

void desenhaCor(){
    Image img(300,300);
    Color
        A = {255, 0, 0},
        B = {0, 255, 0},
        C = {0, 0, 255},
        D = {255, 0, 255};


    for(int x = 0; x < 300; x++){
      for(int y = 0; y < 300; y++){
       for(float u=0; u<=1; u++){
         for(float v=0; v<=1; v++){
            img(x,y)=bilerp(u,v,A,B,C,D);
          }
        }
      }
    }
    img.savePNG("cores.png");
}

void desenhaTabuleiro(){
    Image img(600, 600);


    for(int y = 0; y < 600; y++)
        for(int x = 0; x < 600; x++){
		if(x/75%2==0 || y/75%2==0 )
         img(x,y) = white;
        if(x/75%2==0 && y/75%2==0)
         img(x,y) = black;

    }

    img.savePNG("checkerboard.png");

}

void desenhaTarget(){
    Image img2(600, 600);

    for(int y = 0; y < 600; y++)
        for(int x = 0; x < 600; x++){
           int d = distRaio(x,y);
            if((d/60)%2 != 0)
                img2(x,y) = white;
        }

    img2.savePNG("target.png");

}
