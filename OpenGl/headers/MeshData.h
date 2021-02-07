#ifndef MESH_DATA_H
#define MESH_DATA_H

#include "GLutils.h"
#include "ObjMesh.h"

struct MeshData{
	int n_verts;
	VAO vao;
	GLBuffer vbo, ebo;
	GLTexture texture;
	GLTexture normal_map;
	mat4 Model;
	vec3 pmin, pmax;

	MeshData() = default;

	MeshData(const char* objfile){
		ObjMesh mesh{objfile};
		mesh.boundingBox(pmin, pmax);

		std::vector<ObjMesh::Vertex> vertices;
		std::vector<unsigned int> indices;
		mesh.getLists(vertices, indices);
		make_mesh(vertices, indices);
	}

	template<class Vertex>
	void make_mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices){
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

	void init_texture(const char* image){
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

	void init_normal_texture(const char* image){
		normal_map = GLTexture{GL_TEXTURE_2D};
		normal_map.load(image);

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

	void draw(){
		glBindVertexArray(vao);
		glBindTexture(texture.target, texture);
		glDrawElements(GL_TRIANGLES, n_verts, GL_UNSIGNED_INT, 0);
	}
};

MeshData surface_mesh(int m, int n){
	int N = m*n;

	float u0 = -5, u1 = 5, du = (u1-u0)/(m-1),
	      v0 = -5, v1 = 5, dv = (v1-v0)/(n-1);

	vector<ObjMesh::Vertex> V(N);
	for(int i = 0; i < m; i++){
		for(int j = 0; j < n; j++){
			float u = u0 + i*du;
			float v = v0 + j*dv;

			size_t ij = i + j*m;
			V[ij].position = {cos(u)*sin(v), sin(u)*sin(v), cos(v)};

			float s = i/(m-1.0);
			float t = j/(n-1.0);

			V[ij].texCoords = {s, t};


			vec3 du = {-sin(u)*sin(v), cos(u)*sin(v), 0};
            vec3 dv = {cos(v)*cos(u), cos(v)*sin(u), -sin(v)};

			V[ij].normal = normalize(cross(du, dv));
		}
	}

	vector<unsigned int> indices;
	for(int i = 0; i < m-1; i++){
		for(int j = 0; j < n-1; j++){
			unsigned int ij = i + j*m;
			indices.insert(indices.end(), {
					ij, ij+1, ij+m,
					ij+m+1, ij+m, ij+1
					});
		}
	}

	MeshData mesh;
	mesh.make_mesh(V, indices);
	return mesh;
}

#endif
