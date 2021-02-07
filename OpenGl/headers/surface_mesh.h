#include "MeshBuffers.h"

inline MeshBuffers surface_mesh(int m, int n){
	int N = m*n;

	float u0 = -5, u1 = 5, du = (u1-u0)/(m-1),
	      v0 = -5, v1 = 5, dv = (v1-v0)/(n-1);

	struct Vertex{
		vec3 position;
		vec2 texCoords;
		vec3 normal;
	};
	vector<Vertex> V(N);
	for(int i = 0; i < m; i++){
		for(int j = 0; j < n; j++){
			float u = u0 + i*du;
			float v = v0 + j*dv;

			size_t ij = i + j*m;
			V[ij].position = {u, v, sin(u*v/4)};

			float s = i/(m-1.0);
			float t = j/(n-1.0);

			V[ij].texCoords = {s, t};

			vec3 du = {1, 0, cos(u*v/4)*v/4};
			vec3 dv = {0, 1, cos(u*v/4)*u/4};
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

	return MeshBuffers{V, indices};
}

