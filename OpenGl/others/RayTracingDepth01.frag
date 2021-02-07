#version 330
out vec4 FragColor;

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

const float lightRadius = 2.0;
Sphere spheres[] = Sphere[](
	Sphere(2, vec3(4, 4, 13),   vec4(1.0, 1.0, 1.0, 1), SpecularType),
	Sphere(2, vec3(-5, 7, -6), vec4(1.0, 0.2, 0.5, 1), DiffuseType),
	Sphere(4, vec3(12, 3, -22), vec4(0.8, 1.0, 1.0, 1), DiffuseType),
	Sphere(1, vec3(9, 7, 2),   vec4(0.5, 0.3, 1.0, 1), DiffuseType),
	Sphere(2, vec3(-9, 7, 8), vec4(0.3, 1.0, 0.0, 1), DiffuseType),
	Sphere(3, vec3(15, 3, -9),   vec4(0.6, 0.5, 0.7, 1), SpecularType)
);

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

void main(){
	Ray r = Ray(vec3(0,0,0), normalize(fragPos));	

	Intersection I = calculateIntersection(r);
	if(!I.exists)
		discard;

	vec4 proj = Projection*vec4(I.pos, 1);
	float depth = 0.5*(proj.z/proj.w + 1);
	gl_FragDepth = depth;
}
