
#pragma once
#include "glmath.h"
#include "Color.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "PhysBody3D.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

class Primitive
{
public:

	Primitive();

    void Update();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	mat4x4 transform;
	bool axis,wire;

    PhysBody3D body;

protected:
	PrimitiveTypes type;
};

// ============================================
class CubeP : public Primitive
{
public :
	CubeP();
	CubeP(float sizeX, float sizeY, float sizeZ);
	vec3 GetSize() const;
	void InnerRender() const;
public:
	vec3 size;
};

// ============================================
class SphereP : public Primitive
{
public:
	SphereP();
	SphereP(float radius);
    SphereP(float _radius, float mass);
    float GetRadius() const;
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class CylinderP : public Primitive
{
public:
	CylinderP();
	CylinderP(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class LineP : public Primitive
{
public:
	LineP();
	LineP(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class PlaneP : public Primitive
{
public:
	PlaneP();
	PlaneP(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};