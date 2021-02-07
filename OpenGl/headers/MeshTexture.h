#ifndef MESH_DATA_H
#define MESH_DATA_H

#include "GLutils.h"

struct MeshTexture{
	GLTexture texture;

	MeshTexture() = default;

	MeshTexture(const char* image){
		texture = GLTexture{GL_TEXTURE_2D};
		texture.load(image);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if(GLEW_EXT_texture_filter_anisotropic){
			GLfloat fLargest;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
		}
	}
	
	void bind(){	
		glBindTexture(texture.target, texture);
	}
};

#endif
