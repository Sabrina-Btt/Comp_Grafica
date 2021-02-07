#include "ObjMesh.h"
#include "MeshBuffers.h"

MeshBuffers obj_buffers(const char* filename){
	ObjMesh obj{filename};
	auto L = obj.getLists();
	return MeshBuffers(L.vertices, L.indices);
}
