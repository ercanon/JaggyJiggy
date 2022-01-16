#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -10.0f, 0.0f) 
#define MASS 0

class GameObject;
class ComponentMesh;
class ComponentCollider;
struct PhysVehicle3D;
struct VehicleInfo;

enum class SHAPE_TYPE {
	none = -1,
	BOX,
	CAPSULE,
	SPHERE,
	CONVEX
};

class ModulePhysics3D : public Module
{
public:
    ModulePhysics3D(Application* app, bool start_enabled = true);
	~ModulePhysics3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

    void AddBodyToWorld(btRigidBody* body);

    void RemoveBodyFromWorld(btRigidBody* body);

	ComponentCollider* AddRigidBody(OBB& box, GameObject* gameObject, float mass = 1.0f);
	ComponentCollider* AddRigidBody(Sphere& sphere, GameObject* gameObject, float mass = 1.0f);
	ComponentCollider* AddRigidBody(Capsule& capsule, GameObject* gameObject, float mass = 1.0f);

	ComponentCollider* AddCube(const CubeP& cube, float mass);
	PhysVehicle* AddVehicle(const VehicleInfo& info);

	void CreateTestConstraint();
	void DeleteTestConstraint();
	void ShootBall();
	void ClearBalls();

	void PhysicalizeScene(GameObject* root);
	void AddConstraintP2P(ComponentCollider& bodyA, ComponentCollider& bodyB, const vec3& anchorA, const vec3& anchorB);
	void AddConstraintHinge(ComponentCollider& bodyA, ComponentCollider& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisS, const vec3& axisB, bool disable_collision = false);

	std::vector<btCollisionShape*> shapes;
	std::vector<btDefaultMotionState*> motions;
	std::vector<PhysVehicle*> vehicles;
	std::vector<btTypedConstraint*> constraints;
	std::vector<ComponentCollider*> rigidBodies;

	std::vector<ComponentCollider*> balls;
	std::vector<SphereP*> spheres;
	std::vector<btRigidBody*> physBalls;

	btDiscreteDynamicsWorld*	world;
	bool debug = false;

	// Debug Drawer
	//void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	//void reportErrorWarning(const char* warningString);
	//void draw3dText(const btVector3& location, const char* textString);
	//void setDebugMode(int debugMode);
	//int	 getDebugMode() const;
	//
	//btIDebugDraw::DebugDrawModes mode;
	//Line* line;

private:

	btDefaultCollisionConfiguration*	collision_conf = nullptr;
	btCollisionDispatcher*				dispatcher = nullptr;
	btBroadphaseInterface*				broad_phase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDefaultVehicleRaycaster*			vehicle_raycaster = nullptr;

	ComponentCollider* tester = nullptr;

	// Elements for the test constraints
	CubeP* right_cube = nullptr;
	CubeP* left_cube = nullptr;
	ComponentCollider* right_body = nullptr;
	ComponentCollider* left_body = nullptr;
};