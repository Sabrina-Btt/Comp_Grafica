namespace ColorShader{
	struct Varying{
		vec4 position;
		vec3 color;
	};
	Varying operator*(float t, Varying V){
		return {t*V.position, t*V.color};
	}
	Varying operator+(Varying U, Varying V){
		return {U.position + V.position, U.color + V.color};
	}

	static mat4 M;

	template<typename Vertex>
	void vertexShader(Vertex in, Varying& out){
		out.position = M*toVec4(in.position);
		out.color = toVec3(in.color);
	}

	void fragmentShader(Varying V, Color& FragColor){
		FragColor = toColor(V.color);
	}

}
