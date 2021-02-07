#version 330
out vec4 FragColor;

uniform samplerCube cubemap;

uniform mat4 Projection;
uniform mat4 View;

in vec3 fragPos;

struct Ray{
	vec3 origin;
	vec3 direction;
};

struct Sphere{
	float radius;
	vec3 center;
	vec4 color;
	int type;
};

struct Intersection{
	bool exists;
	vec3 pos;
	vec3 normal;
	float t;
	Sphere sphere;
};

const int DiffuseType = 1;
const int SpecularType = 2;
const int LightType = 3;

const float Radius = 1e5;
//const vec3 lightPos = vec3(0, 9, -18);
uniform vec3 lightPos;

const float lightRadius = 1.0;
Sphere spheres[] = Sphere[](
	Sphere(2, vec3(4, 4, 13),   vec4(1.0, 1.0, 1.0, 1), SpecularType),
	Sphere(2, vec3(-5, 7, -6), vec4(1.0, 0.2, 0.5, 1), DiffuseType),
	Sphere(4, vec3(12, 3, -22), vec4(0.8, 1.0, 1.0, 1), DiffuseType),
	Sphere(1, vec3(9, 7, 2),   vec4(0.5, 0.3, 1.0, 1), DiffuseType),
	Sphere(2, vec3(-9, 7, 8), vec4(0.3, 1.0, 0.0, 1), DiffuseType),
	Sphere(3, vec3(15, 3, -9),   vec4(0.6, 0.5, 0.7, 1), SpecularType),
	Sphere(lightRadius, lightPos,vec4(1.0, 1.0, 1.0, 1), LightType)
);

float rand(float x){
    return fract(sin(x)*43758.5453);
}

float rand(vec2 co){
    return fract(sin(dot(co,vec2(12.9898,78.233))) * 43758.5453);
}

// Menor solucao positiva da equacao ax^2 + bx + c = 0
// Retorna -1 se nao houver solucao positiva
float smallerT(float a, float b, float c){
	float delta = b*b - 4*a*c;
	const float EPS = 1e-50;
	if(delta < -EPS)
		return -1.0;

	if(delta < EPS){
		float t = -b/(2*a);
		if(t > 0)
			return t;
	}else{
		float t1 = (-b-sqrt(delta))/(2*a);
		float t2 = (-b+sqrt(delta))/(2*a);
	
		if(t1 > 0 && t1 < t2)
			return t1;
		else if(t2 > 0 && t2 < t1)
			return t2;
	}

	return -1.0;
}

Intersection intersection(Ray ray, Sphere sphere){
	Intersection I;
	I.exists = false;

	vec3 center = vec3(View*vec4(sphere.center, 1));
	vec3 dif = ray.origin - center;
	
	float a = dot(ray.direction, ray.direction);
	float b = 2*dot(dif, ray.direction);
	float c = dot(dif, dif) - sphere.radius*sphere.radius;
	
	float t = smallerT(a, b, c);
	if(t > 0){
		I.exists = true;
		vec3 P = ray.origin + t*ray.direction;
		I.pos = P;
		I.normal = normalize(P - center);
		I.t = t;
		I.sphere = sphere;
	}

	return I;
}

Intersection calculateIntersection(Ray r){
	Intersection I;
	I.exists = false;
	for(int i = 0; i < spheres.length(); i++){
 		Intersection Ii = intersection(r, spheres[i]);
		if(Ii.exists && (!I.exists || Ii.t < I.t))
			I = Ii;	
	}
	return I;
}

float calculate_hard_shadow(vec3 lightPos, Intersection I){
	vec3 lightDir = normalize(lightPos - I.pos);
	
	Ray lightRay = Ray(I.pos, lightDir);
	Intersection Il = calculateIntersection(lightRay);

	if(Il.exists && Il.sphere.type != LightType)
		return 0.0;
	else
		return 1.0;
}

mat3 getTBN(vec3 N){
	vec3 T = cross(N, vec3(1, 0, 0));
	if(length(T) < 1e-5)
		T = cross(N, vec3(0, 1, 0));
	T = normalize(T);
	vec3 B = cross(N, T);
	return mat3(T, B, N);
}

vec3 circle_point(float r, float theta){
	return vec3(r*cos(theta), r*sin(theta), 0);
}

float calculate_shadow(vec3 lightPos, Intersection I){
	vec3 lightDir = normalize(lightPos - I.pos);
	mat3 TBN = getTBN(lightDir);

	float sum = 0.0;
	int m = 3;
	int n = 0;
	float dr = 1.0/m;
	for(int j = 0; j < m; j++){
		float ratio = j*dr;
		float radius = ratio*lightRadius;
		int nc = 1 + int(20*ratio);
		n += nc;
		float dt = 2*3.14159/nc;
		for(int i = 0; i < nc; i++){
			float theta = i*dt + (rand(I.pos.xy)-0.5)*dt;
			float rr = radius + (rand(I.pos.zy)-0.5)*dr;
			vec3 dif = TBN*circle_point(rr, theta);

			vec3 lightDir = normalize(lightPos + dif - I.pos);
			Ray lightRay = Ray(I.pos, lightDir);
			Intersection Il = calculateIntersection(lightRay);

			if(!Il.exists || Il.sphere.type == LightType)
				sum += 1.0;
		}
	}

	return sum/n;
}

vec4 illumination(vec3 viewDir, vec3 lightPos, Intersection I){
	if(I.sphere.type == LightType)
		return I.sphere.color;

	vec3 lightDir = normalize(lightPos - I.pos);
	
	vec3 normal = I.normal;
	vec4 material_color = I.sphere.color;

	vec4 ambient = 0.2*material_color;
	vec4 diffuse = max(0, dot(lightDir, normal))*material_color;

	//float shadow = 1;
	//float shadow = calculate_hard_shadow(lightPos, I);
	float shadow = calculate_shadow(lightPos, I);

	return ambient + shadow*diffuse;
}

vec4 mirror(Intersection I, Ray r, vec3 light){
	vec4 Color = vec4(0, 0, 0, 0);
	float s = 0.5;
	for(int i = 0; i < 4; i++){
		if(I.sphere.type != SpecularType)
			break;

		r.origin = I.pos;
		r.direction = reflect(r.direction, I.normal);
		I = calculateIntersection(r);
		if(I.exists){
			Color += s*illumination(-r.direction, light, I);
		}else{
			Color += s*texture(cubemap, r.direction);
			break;
		}
		s /= 2;
	}
	return Color;
}

void main(){
	Ray r = Ray(vec3(0,0,0), normalize(fragPos));	

	Intersection I = calculateIntersection(r);
	if(!I.exists)
		discard;

	vec3 light = vec3(View*vec4(lightPos, 1));
	FragColor = illumination(-r.direction, light, I) + mirror(I, r, light);
	
	vec4 proj = Projection*vec4(I.pos, 1);
	gl_FragDepth = 0.5*(proj.z/proj.w + 1);
}
