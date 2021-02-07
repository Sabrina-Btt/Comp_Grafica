#version 330

uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

uniform float mat_shininess;

uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;

uniform mat4 View;

in vec3 position;
in vec3 normal;
in vec2 texCoords;

out vec4 FragColor;

void main(){     
	vec3 N = normalize(normal);

	vec4 mat_color = texture(diffuse_map, texCoords);
	vec4 mat_specular = texture(specular_map, texCoords);
	
	// direção do observador
	vec3 E = normalize(-position); 
	
	// troca a direção da normal caso seja uma backface
	if(!gl_FrontFacing)
		N = -N;

	// Posição da luz no referencial do observador
	vec4 lightPos = View*light_position;

	// Direção da luz
	vec3 L = (lightPos.w == 0)?
		normalize(lightPos.xyz): // luz direcional
		normalize(lightPos.xyz - position); // luz pontual

	// direção do raio refletido
	vec3 R = normalize(reflect(-L, N)); 

	vec4 ambient = mat_color*light_ambient;
	vec4 diffuse = mat_color*light_diffuse*max(0, dot(L, N));
	vec4 specular = mat_specular*light_specular*pow(max(0,dot(R,E)), mat_shininess);

	FragColor = ambient + diffuse + specular;
}
