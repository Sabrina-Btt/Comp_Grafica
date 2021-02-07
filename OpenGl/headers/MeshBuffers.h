#ifndef MESH_BUFFERS_H
#define MESH_BUFFERS_H

#include "GLutils.h"

struct MeshBuffers{
	int n_verts;
	VAO vao;
	GLBuffer vbo, ebo;

	MeshBuffers() = default;

	template<class Vertex>
	MeshBuffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices){
		vao = VAO{true};
		glBindVertexArray(vao);

		vbo = GLBuffer{GL_ARRAY_BUFFER};
		vbo.data(vertices, GL_STATIC_DRAW);

		size_t stride = sizeof(Vertex);
		size_t offset_position = offsetof(Vertex, position);
		size_t offset_texCoords = offsetof(Vertex, texCoords);
		size_t offset_normal = offsetof(Vertex, normal);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_position);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,(void*)offset_texCoords);
		
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,(void*)offset_normal);

		ebo = GLBuffer{GL_ELEMENT_ARRAY_BUFFER};
		ebo.data(indices, GL_STATIC_DRAW);

		n_verts = indices.size();
	}

	void draw(){
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, n_verts, GL_UNSIGNED_INT, 0);
	}
};

#endif
