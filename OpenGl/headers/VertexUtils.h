#ifndef VECCOL_H
#define VECCOL_H

#include "vec.h"
#include "Color.h"

template<class Vec, class Col>
struct PosCol{
	Vec position;
	Col color;
};

template<class Vec, class Col>
PosCol<Vec, Col> lerp(float t, PosCol<Vec, Col> u, PosCol<Vec, Col> v){
	return { lerp(t, u.position, v.position), lerp(t, u.color, v.color) };
}

using Vec2Col = PosCol<vec2, Color>;
using Vec3Col = PosCol<vec3, Color>;
using Vec4Col = PosCol<vec4, Color>;

template<typename Vertex>
vec4 getPosition(Vertex in){
	return toVec4(in.position);
}

inline vec4 getPosition(vec3 v){
	return toVec4(v);
}


#endif
