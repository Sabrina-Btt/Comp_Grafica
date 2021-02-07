#include "Image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Image::Image(const char* filename){
	int nrChannels;
	stbi_set_flip_vertically_on_load(true);
	Color* image = (Color*)stbi_load(filename, &width, &height, &nrChannels, 3);
	if(image){
		pixels.assign(image, image+width*height);
		stbi_image_free(image);
	}
}
	

void Image::savePNG(const char* filename){
	stbi_flip_vertically_on_write(true);
	stbi_write_png(filename, width, height, 3, pixels.data(), 0);
}

void Image::saveBMP(const char* filename){
	stbi_flip_vertically_on_write(true);
	stbi_write_bmp(filename, width, height, 3, pixels.data());
}


