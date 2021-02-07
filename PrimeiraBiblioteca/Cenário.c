#include "Image.h"
#include "ImageRGBA.h"


Image construtor(Image bg, ImageRGBA I, int posy, int posx){
    vec4 vcI;
    vec3 aux;

    for(int y = posy; y < I.height+posy; y++){
        for(int x = posx; x < I.width+posx; x++){
            if(y<bg.height && y>=0 && x>=0 && x<bg.width){  //verifica se o pixel se limita ao intervalo da imagem de fundo
                RGBA cI = I(x-posx,y-posy);
                vcI=toVec4(cI);
                if (vcI[3]!=0){                           //verifica canal alpha do pixel
                    aux = {vcI[0], vcI[1], vcI[2]};
                    Color auxc = toColor(aux);
                    bg(x,y) = lerp(vcI[3], bg(x,y), auxc); //interpolação com o canal alfa
                }
            }
        }
    }
    return bg;
}

int main(){
	Image bg("wintertileset/png/BG/BG.png");

	//background - size - 1800/893;

	//Tiles
	ImageRGBA tile1("wintertileset/png/Tiles/5_5.png");
	ImageRGBA tile2("wintertileset/png/Tiles/6_6.png");
	ImageRGBA tile3("wintertileset/png/Tiles/2_2.png");
	ImageRGBA tile4("wintertileset/png/Tiles/3_3.png");
	ImageRGBA tile5("wintertileset/png/Tiles/1_1.png");
	ImageRGBA tile6("wintertileset/png/Tiles/4_4.png");
	ImageRGBA water1("wintertileset/png/Tiles/17_17.png");
	ImageRGBA water2("wintertileset/png/Tiles/18_18.png");
	ImageRGBA sky1("wintertileset/png/Tiles/14_14.png");
	ImageRGBA sky2("wintertileset/png/Tiles/15_15.png");
	ImageRGBA sky3("wintertileset/png/Tiles/16_16.png");


    //Objects
	ImageRGBA sw("wintertileset/png/Object/SnowMan1.png");
	ImageRGBA sign("wintertileset/png/Object/Sign_2.png");
	ImageRGBA sign2("wintertileset/png/Object/Sign_1.png");
	ImageRGBA crystal("wintertileset/png/Object/Crystal.png");
	ImageRGBA box("wintertileset/png/Object/Crate1.png");
	ImageRGBA tree("wintertileset/png/Object/Tree_1.png");
	ImageRGBA tree1("wintertileset/png/Object/Tree_2.png");
	ImageRGBA igloo("wintertileset/png/Object/Igloo1.png");


	//Water
	bg=construtor(bg,water2,0,200);
    bg=construtor(bg,water1,100,200);

    bg=construtor(bg,water2,0,300);
    bg=construtor(bg,water1,100,300);

    bg=construtor(bg,water2,0,400);
    bg=construtor(bg,water1,100,400);

    bg=construtor(bg,water2,0,500);
    bg=construtor(bg,water1,100,500);

    bg=construtor(bg,water2,0,600);
    bg=construtor(bg,water1,100,600);

    bg=construtor(bg,water2,0,700);
    bg=construtor(bg,water1,100,700);


    //Water
    bg=construtor(bg,water2,-50,1100);
    bg=construtor(bg,water2,50,1100);
    bg=construtor(bg,water2,100,1100);
    bg=construtor(bg,water1,150,1100);

    bg=construtor(bg,water2,-50,1200);
    bg=construtor(bg,water2,50,1200);
    bg=construtor(bg,water2,100,1200);
    bg=construtor(bg,water1,150,1200);

    bg=construtor(bg,water2,-50,1300);
    bg=construtor(bg,water2,50,1300);
    bg=construtor(bg,water2,100,1300);
    bg=construtor(bg,water1,150,1300);




    //Start
    bg=construtor(bg,tile1,0,0);
    bg=construtor(bg,tile1,0,100);
    bg=construtor(bg,tile2,0,200);

    bg=construtor(bg,tile1,100,0);
    bg=construtor(bg,tile1,100,100);
    bg=construtor(bg,tile2,100,200);

    bg=construtor(bg,tile3,200,0);
    bg=construtor(bg,tile3,200,100);
    bg=construtor(bg,tile4,200,200);

    bg=construtor(bg,tree1,300,0);
    bg=construtor(bg,sw,300,150);



    //Plataform
    bg=construtor(bg,sky1,460,350);
    bg=construtor(bg,sky2,460,450);
    bg=construtor(bg,sky3,460,550);
    bg=construtor(bg,sign,(460+sky2.height),390);
    bg=construtor(bg,crystal,(460+sky2.height),500);


    //Middle
    bg=construtor(bg,tile6,-50,700);
    bg=construtor(bg,tile1,-50,800);
    bg=construtor(bg,tile1,-50,900);
    bg=construtor(bg,tile1,-50,1000);
    bg=construtor(bg,tile2,-50,1100);

    bg=construtor(bg,tile6,50,700);
    bg=construtor(bg,tile1,50,800);
    bg=construtor(bg,tile1,50,900);
    bg=construtor(bg,tile1,50,1000);
    bg=construtor(bg,tile2,50,1100);

    bg=construtor(bg,tile6,150,700);
    bg=construtor(bg,tile1,150,800);
    bg=construtor(bg,tile1,150,900);
    bg=construtor(bg,tile1,150,1000);
    bg=construtor(bg,tile2,150,1100);

    bg=construtor(bg,tile5,250,700);
    bg=construtor(bg,tile3,250,800);
    bg=construtor(bg,tile3,250,900);
    bg=construtor(bg,tile3,250,1000);
    bg=construtor(bg,tile4,250,1100);

    bg=construtor(bg,box,350,730);
    bg=construtor(bg,tree,350,850);




    //Finish
    bg=construtor(bg,tile6,0,1300);
    bg=construtor(bg,tile1,0,1400);
    bg=construtor(bg,tile1,0,1500);
    bg=construtor(bg,tile1,0,1600);
    bg=construtor(bg,tile1,0,1700);

    bg=construtor(bg,tile6,100,1300);
    bg=construtor(bg,tile1,100,1400);
    bg=construtor(bg,tile1,100,1500);
    bg=construtor(bg,tile1,100,1600);
    bg=construtor(bg,tile1,100,1700);

    bg=construtor(bg,tile6,200,1300);
    bg=construtor(bg,tile1,200,1400);
    bg=construtor(bg,tile1,200,1500);
    bg=construtor(bg,tile1,200,1600);
    bg=construtor(bg,tile1,200,1700);

    bg=construtor(bg,tile6,300,1300);
    bg=construtor(bg,tile1,300,1400);
    bg=construtor(bg,tile1,300,1500);
    bg=construtor(bg,tile1,300,1600);
    bg=construtor(bg,tile1,300,1700);

    bg=construtor(bg,tile5,400,1300);
    bg=construtor(bg,tile3,400,1400);
    bg=construtor(bg,tile3,400,1500);
    bg=construtor(bg,tile3,400,1600);
    bg=construtor(bg,tile3,400,1700);

    bg=construtor(bg,igloo,500,1500);
    bg=construtor(bg,sign2,500,1360);



    bg.savePNG("Trabalho_Sabrina.png");

}



