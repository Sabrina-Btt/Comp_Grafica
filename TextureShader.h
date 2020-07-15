
#ifndef TEXTURE_SHADER_H
#define TEXTURE_SHADER_H

#include "Sampler2D.h"

namespace TextureShader{
	struct Varying{
		vec4 position;
		vec2 texCoords;
	};
	Varying operator*(float t, Varying V){
		return {t*V.position, t*V.texCoords};
	}
	Varying operator+(Varying U, Varying V){
		return {U.position + V.position, U.texCoords + V.texCoords};
	}

	static mat4 M;
	static Sampler2D texture;

	template<class Vertex>
	void vertexShader(Vertex in, Varying& out){
		out.position = M*toVec4(in.position);
		out.texCoords = in.texCoords;
	}

	void fragmentShader(Varying V, Color& FragColor){
		FragColor = texture.sample(V.texCoords);
	}
}

#endif
