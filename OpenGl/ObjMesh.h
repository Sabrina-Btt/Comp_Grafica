#ifndef READ_OBJ_H
#define READ_OBJ_H

#include <vector>
#include <iostream>
#include <map>

struct ObjMesh{
	struct Vertex{
		vec3 position;
		vec2 texCoords;
		vec3 normal;
	};
	struct VertIndices{
		int pos = -1;
		int tex = -1;
		int nor = -1;

		friend std::istream& operator>>(std::istream& in, VertIndices& v){
			in >> v.pos;
			if(in.eof())
				return in;
			v.tex = -1;
			v.nor = -1;
			if(in.peek() == '/'){
				in.ignore();

				if(in.peek() != '/')
					in >> v.tex;

				if(in.eof())
					return in;
				if(in.peek() == '/'){
					in.ignore();
					in >> v.nor;
				}
			}
			return in;
		}

		bool operator<(VertIndices other) const{
			if(pos < other.pos)
				return true;
			if(pos > other.pos)
				return false;
			if(tex < other.tex)
				return true;
			if(tex > other.tex)
				return false;
			if(nor < other.nor)
				return true;
			return false;
		}
	};

	struct Face{
		std::vector<VertIndices> verts;
		friend std::istream& operator>>(std::istream& in, Face& f){
			VertIndices v;
			while(in >> v)
				f.verts.push_back(v);
			return in;
		}
	};

	std::vector<vec3> position;
	std::vector<vec3> normal;
	std::vector<vec2> texCoords;
	std::vector<Face> faces;

	ObjMesh(const char* filename){
		std::ifstream in(filename);
		std::string line;
		while(getline(in, line))
			parseLine(line);
	}

	void parseLine(std::string line){
		std::string op;
		std::istringstream in(line);
		in >> op;
		if(op == "v"){
			vec3 v = {0, 0, 0};
			in >> v.x >> v.y >> v.z;
			position.push_back(v);
		}if(op == "vn"){
			vec3 v = {0, 0, 0};
			in >> v.x >> v.y >> v.z;
			normal.push_back(v);
		}if(op == "vt"){
			vec2 v = {0, 0};
			in >> v.x >> v.y;
			texCoords.push_back(v);
		}else if(op == "f"){
			Face f;
			in >> f;
			faces.push_back(f);
		}
	}

	Vertex getVertex(VertIndices v)const{
		Vertex vert;
		vert.position = {position[v.pos-1]};
		if(v.tex > 0)
			vert.texCoords = texCoords[v.tex-1];
		if(v.nor > 0)
			vert.normal = normal[v.nor-1];
		return vert;
	}

	struct Lists{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	Lists getLists()const{
		Lists L;

		int count = 0;
		std::map<VertIndices, int> M;
		for(const Face& f: faces){
			std::vector<unsigned int> face_indices;
			for(VertIndices v: f.verts){
				auto p = M.insert({v, count});
				if(p.second){
					count++;
					L.vertices.push_back(getVertex(v));
				}
				auto it = p.first;
				face_indices.push_back(it->second);
			}
			for(size_t i = 0; i < face_indices.size()-2; i++){
				// Triangle fan
				L.indices.push_back(face_indices[0]);
				L.indices.push_back(face_indices[i+1]);
				L.indices.push_back(face_indices[i+2]);
			}
		}

		return L;
	}

	template<class T>
	void getLists(
		std::vector<Vertex>& vertices,
		std::vector<T>& indices
	)const{
		vertices.clear();
		indices.clear();

		int count = 0;
		std::map<VertIndices, int> M;
		for(const Face& f: faces){
			std::vector<T> face_indices;
			for(VertIndices v: f.verts){
				auto p = M.insert({v, count});
				if(p.second){
					count++;
					vertices.push_back(getVertex(v));
				}
				auto it = p.first;
				face_indices.push_back(it->second);
			}
			for(size_t i = 0; i < face_indices.size()-2; i++){
				// Triangle fan
				indices.push_back(face_indices[0]);
				indices.push_back(face_indices[i+1]);
				indices.push_back(face_indices[i+2]);
			}
		}
	}

	vec3 center(){
		vec3 C = {0, 0, 0};
		for(vec3 v: position)
			C = C + v;
		return (1.0f/position.size())*C;
	}

	void boundingBox(vec3& pmin, vec3& pmax){
		if(position.empty())
			return;
		pmin = pmax = position[0];
		for(vec3 v: position){
			for(int k = 0; k < 3; k++){
				if(v[k] < pmin[k])
					pmin[k] = v[k];
				if(v[k] > pmax[k])
					pmax[k] = v[k];
			}
		}
	}
};

#endif
