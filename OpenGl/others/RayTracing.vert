#version 330
layout (location = 0) in vec4 Position;

uniform mat4 Projection;

out vec3 fragPos;

void main(){
	fragPos = vec3(inverse(Projection)*Position);

	gl_Position = Position;
} 
