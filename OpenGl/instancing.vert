#version 330

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 S;
uniform mat4 R;
uniform int N;
uniform bool offset_test;

layout (location = 0) in vec4 Position;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 offset;

out vec2 texCoords;

void main(){
	int i = gl_InstanceID/N;
	int j = gl_InstanceID%N;
	mat4 T = mat4(1);

    if(offset_test)
        T[3].xyz = vec3(i-N/2, j-N/2, 0) + offset;
    else
        T[3] = vec4((i-N)*2, (j-N)*2, 0, 1);

	mat4 Model = R*T*S;

	gl_Position = Projection*View*Model*Position;

	texCoords = TexCoords;
}


