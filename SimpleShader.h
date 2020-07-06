#ifndef SIMPLE_SHADER_H
#define SIMPLE_SHADER_H

namespace SimpleShader{
	struct Varying{
		vec4 position;
	};
	Varying operator*(float t, Varying V){
		return {t*V.position};
	}

	Varying operator+(Varying U, Varying V){
		return {U.position + V.position};
	}

	static mat4 M;
	static Color C;

	template<class Vertex>
	void vertexShader(Vertex in, Varying& out){
		out.position = M*toVec4(in);
	}

	void fragmentShader(Varying, Color& fragColor){
		fragColor = C;
	}
}

#endif
