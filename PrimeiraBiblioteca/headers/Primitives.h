
#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <iostream>
#include <array>
#include <vector>

///////////////////////////////////////////////////////////////////////
template<class V>
using Line = std::array<V, 2>;

struct LineIndices{
	size_t i, j;

};
template<class V>
Line<V> assemble(const std::vector<V>& v, LineIndices l){
	return {v[l.i], v[l.j]};
}

class Lines{
	size_t n;
	public:
	Lines(size_t s): n{s/2}{}

	size_t size() const{ return n; }

	LineIndices operator[](size_t i) const{ return {2*i, 2*i+1}; }
};

class LineStrip{
    size_t n;
	public:
	LineStrip(size_t s): n{s-1}{}

	size_t size() const{ return n; }

	LineIndices operator[](size_t i) const{ return {i, i+1}; }
};

class LineLoop{
    size_t n;
	public:
	LineLoop(size_t s): n{s}{}

	size_t size() const{ return n; }

	LineIndices operator[](size_t i) const{
	    if(i == n-1)
            return {i, i-(n-1)};
        else
            return {i, i+1};
        }
};

///////////////////////////////////////////////////////////////////////
template<class V>
using Triangle = std::array<V, 3>;

struct TriIndices{
	size_t i, j, k;
};

template<class V>
Triangle<V> assemble(const std::vector<V>& v, TriIndices t){
	return {v[t.i], v[t.j], v[t.k]};
}

class Triangles{
	size_t n;
	public:
	Triangles(size_t s): n{s/3}{}

	size_t size() const{ return n; }

	TriIndices operator[](size_t i) const{ return {3*i, 3*i+1, 3*i+2}; }
};

class TriangleStrip{
    size_t n;
	public:
	TriangleStrip(size_t s): n{s-2}{}

	size_t size() const{ return n; }

	TriIndices operator[](size_t i) const{ return {i, i+1, i+2}; }
};

class TriangleFan{
    size_t n;
	public:
	TriangleFan(size_t s): n{s-2}{}

	size_t size() const{ return n; }

	TriIndices operator[](size_t i) const{ return {i-i, i+1, i+2}; }
};

///////////////////////////////////////////////////////////////////////
template<class Primitives>
class Elements{
	std::vector<size_t> indices;
	Primitives P;

	public:
	Elements(const std::vector<size_t>& in): indices{in}, P{in.size()}{}

	size_t size() const{ return P.size(); }

	auto operator[](size_t i) const{
		return getPrimitive(P[i]);
	}

	LineIndices getPrimitive(LineIndices in) const{
		return {indices[in.i], indices[in.j]};
	}

	TriIndices getPrimitive(TriIndices in) const{
		return {indices[in.i], indices[in.j], indices[in.k]};
	}

};

#endif
