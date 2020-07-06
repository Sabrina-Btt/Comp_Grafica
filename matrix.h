#ifndef MATRIX_H
#define MATRIX_H

#include "vec.h"


/*******************************************************************/
/************************ mat2 *************************************/
struct mat2{
	vec2 L[2];

	vec2& operator[](int i){
		return L[i];
	}
};

inline void print(mat2 A){
	print(A[0]);
	print(A[1]);
}

inline mat2 transpose(mat2 m){
	return {
		m[0][0], m[1][0],
		m[0][1], m[1][1]
	};
}

inline mat2 operator*(mat2 A, mat2 B){
	mat2 Bt = transpose(B);
	mat2 C;
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < 2; j++)
			C[i][j] = dot(A[i], Bt[j]);
	return C;
}

inline vec2 operator*(mat2 A, vec2 v){
	return {
		dot(A[0], v),
		dot(A[1], v),
	};
}

inline std::vector<vec2> operator*(mat2 A, std::vector<vec2>& P){
	int n = P.size();
	std::vector<vec2> Q(n);
	for(int i = 0; i < n; i++)
		Q[i] = A*P[i];
	return Q;
}


/*******************************************************************/
/************************ mat3 *************************************/
struct mat3{
	vec3 L[3];

	vec3& operator[](int i){
		return L[i];
	}
};

inline void print(mat3 A){
	print(A[0]);
	print(A[1]);
	print(A[2]);
}

inline mat3 transpose(mat3 m){
	return {
		m[0][0], m[1][0], m[2][0],
		m[0][1], m[1][1], m[2][1],
		m[0][2], m[1][2], m[2][2]
	};
}

inline mat3 operator*(mat3 A, mat3 B){
	mat3 Bt = transpose(B);
	mat3 C;
	for(int i = 0; i < 3; i++)
	for(int j = 0; j < 3; j++)
		C[i][j] = dot(A[i], Bt[j]);
	return C;
}

inline vec3 operator*(mat3 A, vec3 v){
	return {
		dot(A[0], v),
		dot(A[1], v),
		dot(A[2], v)
	};
}

inline std::vector<vec3> operator*(mat3 A, std::vector<vec3>& P){
	int n = P.size();
	std::vector<vec3> Q(n);
	for(int i = 0; i < n; i++)
		Q[i] = A*P[i];
	return Q;
}

// Homogeneous coordinates
inline vec2 operator*(mat3 A, vec2 v){
	return toVec2Homogeneous(A*toVec3(v,1));
}

inline std::vector<vec2> operator*(mat3 A, std::vector<vec2>& P){
	int n = P.size();
	std::vector<vec2> Q(n);
	for(int i = 0; i < n; i++)
		Q[i] = A*P[i];
	return Q;
}

inline mat3 inverse(mat3 m){
	// computes the inverse of a matrix m
	float det =
			m[0][0]*(m[1][1]*m[2][2] - m[2][1]*m[1][2]) -
            m[0][1]*(m[1][0]*m[2][2] - m[1][2]*m[2][0]) +
            m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0]);

	float invdet = 1/det;

	mat3 minv;

	minv[0][0] = (m[1][1]*m[2][2] - m[2][1]*m[1][2])*invdet;
	minv[0][1] = (m[0][2]*m[2][1] - m[0][1]*m[2][2])*invdet;
	minv[0][2] = (m[0][1]*m[1][2] - m[0][2]*m[1][1])*invdet;
	minv[1][0] = (m[1][2]*m[2][0] - m[1][0]*m[2][2])*invdet;
	minv[1][1] = (m[0][0]*m[2][2] - m[0][2]*m[2][0])*invdet;
	minv[1][2] = (m[1][0]*m[0][2] - m[0][0]*m[1][2])*invdet;
	minv[2][0] = (m[1][0]*m[2][1] - m[2][0]*m[1][1])*invdet;
	minv[2][1] = (m[2][0]*m[0][1] - m[0][0]*m[2][1])*invdet;
	minv[2][2] = (m[0][0]*m[1][1] - m[1][0]*m[0][1])*invdet;

	return minv;
}

/*******************************************************************/
/************************ mat4 *************************************/

struct mat4{
	vec4 L[4];

	vec4& operator[](int i){
		return L[i];
	}
};

inline void print(mat4 A){
	for(int i = 0; i < 4; i++)
		print(A[i]);
}

inline mat4 transpose(mat4 m){
	mat4 MT;
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			MT[i][j] = m[j][i];
	return MT;
}

inline mat4 operator*(mat4 A, mat4 B){
	mat4 Bt = transpose(B);
	mat4 C;
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			C[i][j] = dot(A[i], Bt[j]);
	return C;
}

inline vec4 operator*(mat4 A, vec4 v){
	return {
		dot(A[0], v),
		dot(A[1], v),
		dot(A[2], v),
		dot(A[3], v)
	};
}

inline std::vector<vec4> operator*(mat4 A, std::vector<vec4>& P){
	int n = P.size();
	std::vector<vec4> Q(n);
	for(int i = 0; i < n; i++)
		Q[i] = A*P[i];
	return Q;
}

inline mat4 toMat4(mat3 A){
	return {
		toVec4(A[0], 0),
		toVec4(A[1], 0),
		toVec4(A[2], 0),
		0, 0, 0, 1
	};
}

inline mat3 toMat3(mat4 A){
	return {
		toVec3(A[0]),
		toVec3(A[1]),
		toVec3(A[2])
	};
}

inline mat4 loadIdentity(){
	return {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}

//Manipulação de matrizes
mat4 translate(float a, float b, float c){
    return{
        1, 0, 0, a,
        0, 1, 0, b,
        0, 0, 1, c,
        0, 0, 0, 1
    };
}

mat4 scale(float a, float b, float c){
    return{
        a, 0, 0, 0,
        0, b, 0, 0,
        0, 0, c, 0,
        0, 0, 0, 1
    };
}

mat4 rotate_x(float theta){
    return{
        1,     0,          0,       0,
        0, cos(theta), -sin(theta), 0,
        0, sin(theta), cos(theta),  0,
        0,     0,          0,       1
    };
}

mat4 rotate_y(float theta){
    return{
        cos(theta),  0, sin(theta),  0,
            0,       1,     0,       0,
        -sin(theta), 0, cos(theta),  0,
            0,       0,     0,       1
    };
}

mat4 rotate_z(float theta){
    return{
        cos(theta), -sin(theta),  0,  0,
        sin(theta),  cos(theta),  0,  0,
             0,          0,       1,  0,
             0,          0,       0,  1
    };
}



#endif

