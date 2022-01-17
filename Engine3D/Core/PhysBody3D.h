#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2List.h"
#include "glmath.h"

class btRigidBody;
class btCollisionShape;
struct btDefaultMotionState;
class Module;

class SphereP;
class CubeP;
class CylinderP;
class PlaneP;

class Primitive;

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D();
    PhysBody3D(btRigidBody* body);
	~PhysBody3D();

    void SetBody(SphereP* primitive, float mass);

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);

    void SetBody(btCollisionShape* shape, Primitive* parent, float mass);

private:
	btRigidBody* body = nullptr;
    btCollisionShape* colShape;
    btDefaultMotionState* motionState;
public:
    Primitive* parentPrimitive;
	p2List<Module*> collision_listeners;
};

#endif // __PhysBody3D_H__