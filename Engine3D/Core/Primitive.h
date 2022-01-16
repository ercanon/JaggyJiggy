#ifndef __PRIMITIVE_H__
#define	__PRIMITIVE_H__

#include "Globals.h"
#include "glmath.h"
#include "Color.h"

enum class PRIMITIVE_TYPE
{
	POINT,
	LINE,
	PLANE,
	CUBE,
	SPHERE,
	CYLINDER
};

class Primitive
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	PRIMITIVE_TYPE	GetType() const;

public:

	Color color;
	mat4x4 transform;
	bool axis, wire;

protected:
	PRIMITIVE_TYPE type;
};

// ============================================
class PrimitiveCube : public Primitive
{
public:
	PrimitiveCube();
	PrimitiveCube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
};

// ============================================
class PrimitiveSphere : public Primitive
{
public:
	PrimitiveSphere();
	PrimitiveSphere(float radius);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class PrimitiveCylinder : public Primitive
{
public:
	PrimitiveCylinder();
	PrimitiveCylinder(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class PrimitiveLine : public Primitive
{
public:
	PrimitiveLine();
	PrimitiveLine(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class PrimitivePlane : public Primitive
{
public:
	PrimitivePlane();
	PrimitivePlane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};

#endif