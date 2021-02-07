#version 330

uniform mat4 M;
in vec3 v;

void main(){
    gl_Position = M*vec4(v,1);
}
