#version 330

uniform sampler2D shadow_map;
in vec4 vLightSpace;

uniform sampler2D mat_texture;

uniform vec4 mat_specular;
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

float calculateShadow(vec4 pos){
	vec3 projCoords = pos.xyz/pos.w;

	// Transforma para o intervalo [0,1]
	projCoords = projCoords*0.5 + 0.5;

	if(projCoords.z > 1)
		return 1.0; // Ignora pontos além do plano far da camera

	float closestDepth = texture(shadow_map, projCoords.xy).r; 
	float currentDepth = projCoords.z;

	//return currentDepth > closestDepth? 0.0 : 1.0;
	float bias = 0.00001;
	return currentDepth - bias > closestDepth  ? 0.0 : 1.0;  
}



void main(){     
	vec3 N = normalize(normal);

	vec4 mat_color = texture(mat_texture, texCoords);
	
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

	float shadow = calculateShadow(vLightSpace);

	FragColor = ambient + shadow*(diffuse + specular);
}
