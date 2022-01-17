#include "PhysBody3D.h"
#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "Primitive.h"
#include "glmath.h"
#include "btBulletDynamicsCommon.h"

// =================================================
PhysBody3D::PhysBody3D()
    : body(nullptr)
    , colShape(nullptr)
    , motionState(nullptr)
    , parentPrimitive(nullptr)
    , collision_listeners()
{}

PhysBody3D::PhysBody3D(btRigidBody* body)
    : body(nullptr)
    , colShape(nullptr)
    , motionState(nullptr)
    , parentPrimitive(nullptr)
    , collision_listeners()
{
    this->body = body;
}

// ---------------------------------------------------------
PhysBody3D::~PhysBody3D()
{
    if (body != nullptr)
    {
        App->physics->RemoveBodyFromWorld(body);
        delete body;
        delete colShape;
        delete motionState;
    }
}

void PhysBody3D::SetBody(SphereP* primitive, float mass)
{
    SetBody(new btSphereShape(primitive->GetRadius()),
        primitive, mass);
}

void PhysBody3D::SetBody(CubeP* primitive, float mass)
{
    SetBody(new btBoxShape(btVector3(primitive->GetSize().x, primitive->GetSize().y, primitive->GetSize().z)),
        primitive, mass);
}

// ---------------------------------------------------------
void PhysBody3D::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

// ---------------------------------------------------------
void PhysBody3D::GetTransform(float* matrix) const
{
	if(body != NULL && matrix != NULL)
	{
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetTransform(const float* matrix) const
{
	if(body != NULL && matrix != NULL)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
}

void PhysBody3D::SetBody(btCollisionShape* shape, Primitive* parent, float mass)
{
    parentPrimitive = parent;

    colShape = shape;

    btTransform startTransform;
    startTransform.setFromOpenGLMatrix(&parent->transform);

    btVector3 localInertia(0, 0, 0);
    if (mass != 0.f)
        colShape->calculateLocalInertia(mass, localInertia);

    motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);

    body = new btRigidBody(rbInfo);

    body->setUserPointer(this);

    App->physics->AddBodyToWorld(body);
}

void PhysBody3D::RemoveBody()
{
    if (body != nullptr)
    {
        App->physics->RemoveBodyFromWorld(body);
        delete body;
        delete colShape;
        delete motionState;
    }
}
