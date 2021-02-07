
#ifndef SAMPLER2D_H
#define SAMPLER2D_H

enum Filter{NEAREST, BILINEAR};
enum WrapMode{CLAMP, REPEAT, MIRRORED_REPEAT};

struct Sampler2D{
	Image img;
	Filter filter;
	WrapMode wrapX, wrapY;

	int limitValue(int v, int vmax, WrapMode wrap){
		switch(wrap){
			case CLAMP:
				v = clamp(v, 0, vmax-1);
			break;
			case REPEAT: /* TAREFA */
			break;
			case MIRRORED_REPEAT: /* TAREFA */
			break;
		}
		return v;
	}

	Color at(int x, int y){
		x = limitValue(x, img.width, wrapX);
		y = limitValue(y, img.height, wrapY);
		return img(x, y);
	}

	Color sample(vec2 texCoords){
		float sx = texCoords.x*img.width - 0.5;
		float sy = texCoords.y*img.height - 0.5;

		if(filter == BILINEAR)
			return sampleBI(sx, sy);

		return sampleNN(sx, sy);
	}

	Color sampleNN(float sx, float sy){
		int x = round(sx);
		int y = round(sy);
		return at(x, y);
	}

	Color sampleBI(float sx, float sy){
		int x = floor(sx);
		int y = floor(sy);
		float u = sx - x;
		float v = sy - y;
		return bilerp(u, v,
			at(x, y), at(x+1,y),
			at(x, y+1), at(x+1, y+1)
		);
	}
};


#endif
