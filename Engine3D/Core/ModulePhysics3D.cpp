#include "glew.h"
#include "SDL/include/SDL_opengl.h"
#include "PhysVehicle.h"
#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentCollider.h"
#include "Time.h"
#include "stdio.h"

ModulePhysics3D::ModulePhysics3D(Application* app, bool start_enabled) : Module(app, start_enabled), world(nullptr)
{
	debug = false; // NOTA: Quizas se deba sacar.

	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
}

// Destructor
ModulePhysics3D::~ModulePhysics3D()
{
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_conf;
	delete right_cube;
	delete left_cube;
	delete right_body;
	delete left_body;
}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init()
{
	LOG("Creating 3D Physics simulation");
	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	world->setGravity(GRAVITY);
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	LOG("Creating Physics environment");

	// Big plane as ground
	btCollisionShape* colShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

	btDefaultMotionState* myMotionState = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);
	btRigidBody* body = new btRigidBody(rbInfo);
	world->addRigidBody(body);
	//world->setDebugDrawer(debug_draw); // Necesidad de un debug drawer de colliders?

	// Creating cubes for the test constraint
	right_cube = new PrimitiveCube(3, 4, 3);
	left_cube = new PrimitiveCube(3, 4, 3);
	right_cube->SetPos(5, 2, 25);
	left_cube->SetPos(-5, 2, 25);
	right_cube->color.Set(Red.r, Red.g, Red.b);
	left_cube->color.Set(Green.r, Green.g, Green.b);
	right_body = App->physics->AddCube(*right_cube, 25.0f);
	left_body = App->physics->AddCube(*left_cube, 25.0f);

	// Set debug mode to wireframe
	//debug_draw->setDebugMode(1);

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	if (App->editor->play)
	{
		world->stepSimulation(dt, 15);

		int numManifolds = world->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
			btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
			btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

			int numContacts = contactManifold->getNumContacts();
			if (numContacts > 0)
			{
				ComponentCollider* pbodyA = (ComponentCollider*)obA->getUserPointer();
				ComponentCollider* pbodyB = (ComponentCollider*)obB->getUserPointer();

				if (pbodyA && pbodyB)
				{
					std::vector<Module*>::iterator item = pbodyA->collision_listeners.begin();
					while (item != pbodyA->collision_listeners.end())
					{
						(*item)->OnCollision(pbodyA, pbodyB);
						item++;
					}

					item = pbodyA->collision_listeners.begin();
					while (item != pbodyA->collision_listeners.end())
					{
						(*item)->OnCollision(pbodyA, pbodyB);
						item++;
					}
				}
			}
		}
	}

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt)
{
	right_body->GetTransform(&right_cube->transform);
	left_body->GetTransform(&left_cube->transform);
	right_cube->Render();
	left_cube->Render();

	for (int i = 0; i < balls.size(); ++i) {
		balls[i]->GetTransform(&spheres[i]->transform);
		spheres[i]->Render();
	}

	if (debug) world->debugDrawWorld();

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics3D::CleanUp()
{
	LOG("Destroying 3D Physics simulation");

	// Remove from the world all collision bodies
	for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}
	
	// Clear all the constraints
	for (int i = 0; i < constraints.size(); ++i)
	{
		world->removeConstraint(constraints[i]);
		RELEASE(constraints[i]);
	}
	constraints.clear();

	// Clear all the lists
	for (int i = 0; i < motions.size(); ++i)
		RELEASE(motions[i])
		motions.clear();

	for (int i = 0; i < shapes.size(); ++i)
		RELEASE(shapes[i])
		shapes.clear();

	for (int i = 0; i < rigidBodies.size(); ++i)
		RELEASE(rigidBodies[i])
		rigidBodies.clear();

	for (int i = 0; i < vehicles.size(); ++i)
		RELEASE(vehicles[i])
		vehicles.clear();

	// Clear all the debug balls
	ClearBalls();
	delete world;

	return true;
}

// METHODS TO ADD RIGIDBODIES

ComponentCollider* ModulePhysics3D::AddRigidBody(OBB& box, GameObject* gameObject, float mass)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(box.HalfSize().x, box.HalfSize().y, box.HalfSize().z));
	shapes.push_back(colShape);

	btTransform transform; //GetGlobalGLTransform
	transform.setFromOpenGLMatrix(&gameObject->transform->GetGlobalGLTransform());

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);

	ComponentCollider* component = gameObject->GetComponent<ComponentCollider>();
	if (component == nullptr)
		component = gameObject->GetComponent<ComponentCollider>();
	component->mass = mass;
	component->CreateBody(body);
	world->addRigidBody(body);
	body->setUserPointer(component);

	return component;
}

ComponentCollider* ModulePhysics3D::AddRigidBody(Sphere& sphere, GameObject* gameObject, float mass)
{
	btCollisionShape* colShape = new btSphereShape(sphere.r);
	shapes.push_back(colShape);

	btTransform transform;
	transform.setFromOpenGLMatrix(&gameObject->transform->GetGlobalGLTransform());

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);

	ComponentCollider* component = gameObject->GetComponent<ComponentCollider>();
	if (component == nullptr) component = gameObject->GetComponent<ComponentCollider>();
	component->mass = mass;
	component->CreateBody(body);
	world->addRigidBody(body);

	body->setUserPointer(component);
	return component;
}

ComponentCollider* ModulePhysics3D::AddRigidBody(Capsule& capsule, GameObject* gameObject, float mass)
{
	btCollisionShape* colShape = new btCapsuleShape(capsule.r, capsule.Height());
	shapes.push_back(colShape);

	btTransform transform;
	transform.setFromOpenGLMatrix(&gameObject->transform->GetGlobalGLTransform());

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);

	ComponentCollider* component = gameObject->GetComponent<ComponentCollider>();
	if (component == nullptr) component = gameObject->GetComponent<ComponentCollider>();
	component->mass = mass;
	component->CreateBody(body);
	world->addRigidBody(body);

	body->setUserPointer(component);
	return component;
}

// Creates primitive cubes, just for debug purpouses
ComponentCollider* ModulePhysics3D::AddCube(const PrimitiveCube& cube, float mass)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x * 0.5f, cube.size.y * 0.5f, cube.size.z * 0.5f));
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cube.transform);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	ComponentCollider* pbody = new ComponentCollider(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	rigidBodies.push_back(pbody);

	return pbody;
}

// METHODS RELATED WITH THE PLAYER

// Shoots a ball
void ModulePhysics3D::ShootBall()
{
	PrimitiveSphere* sphere = new PrimitiveSphere(0.5);
	sphere->color.Set(Red.r, Red.g, Red.b);
	sphere->SetPos(App->camera->cameraFrustum.pos.x + App->camera->cameraFrustum.front.x,
		App->camera->cameraFrustum.pos.y + App->camera->cameraFrustum.front.y,
		App->camera->cameraFrustum.pos.z + App->camera->cameraFrustum.front.z);
	spheres.push_back(sphere);

	float mass = 1.0f;
	btCollisionShape* colShape = new btSphereShape(sphere->radius);
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&sphere->transform);

	btVector3 localInertia(0, 0, 0);
	colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	ComponentCollider* pbody = new ComponentCollider(body);
	pbody->Push(App->camera->cameraFrustum.front.x * 35, App->camera->cameraFrustum.front.y * 35, App->camera->cameraFrustum.front.z * 35);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	physBalls.push_back(body);
	balls.push_back(pbody);
}

// Cretes a vehicle and adds it to the scene
PhysVehicle* ModulePhysics3D::AddVehicle(const VehicleInfo& info)
{
	btCompoundShape* comShape = new btCompoundShape();
	shapes.push_back(comShape);

	btCollisionShape* colShape = new btBoxShape(btVector3(info.chassis_size.x * 0.5f, info.chassis_size.y * 0.5f, info.chassis_size.z * 0.5f));
	shapes.push_back(colShape);

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z));

	comShape->addChildShape(trans, colShape);

	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0, 0, 0);
	comShape->calculateLocalInertia(info.mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(info.mass, myMotionState, comShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
	body->setActivationState(DISABLE_DEACTIVATION);

	world->addRigidBody(body);

	btRaycastVehicle::btVehicleTuning tuning;
	tuning.m_frictionSlip = info.frictionSlip;
	tuning.m_maxSuspensionForce = info.maxSuspensionForce;
	tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
	tuning.m_suspensionCompression = info.suspensionCompression;
	tuning.m_suspensionDamping = info.suspensionDamping;
	tuning.m_suspensionStiffness = info.suspensionStiffness;

	btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, body, vehicle_raycaster);

	vehicle->setCoordinateSystem(0, 1, 2);

	for (int i = 0; i < info.num_wheels; ++i)
	{
		btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
		btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
		btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);

		vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
	}

	PhysVehicle* phys_vehicle = new PhysVehicle(body, vehicle, info);
	world->addVehicle(vehicle);
	vehicles.push_back(phys_vehicle);

	return phys_vehicle;
}

// Constraint creators
void ModulePhysics3D::AddConstraintP2P(ComponentCollider& bodyA, ComponentCollider& bodyB, const vec3& anchorA, const vec3& anchorB)
{
	btTypedConstraint* p2p = new btPoint2PointConstraint(
		*(bodyA.body),
		*(bodyB.body),
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z));

	world->addConstraint(p2p);
	constraints.push_back(p2p);
	p2p->setDbgDrawSize(2.0f);
}

void ModulePhysics3D::AddConstraintHinge(ComponentCollider& bodyA, ComponentCollider& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisA, const vec3& axisB, bool disable_collision)
{
	btHingeConstraint* hinge = new btHingeConstraint(
		*(bodyA.body),
		*(bodyB.body),
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z),
		btVector3(axisA.x, axisA.y, axisA.z),
		btVector3(axisB.x, axisB.y, axisB.z));

	world->addConstraint(hinge, disable_collision);
	constraints.push_back(hinge);
	hinge->setDbgDrawSize(2.0f);
}

void ModulePhysics3D::CreateTestConstraint()
{
	AddConstraintP2P(*right_body, *left_body, { -5, 0, 0 }, { 5, 0, 0 });
}

void ModulePhysics3D::DeleteTestConstraint()
{
	for (int i = 0; i < constraints.size(); ++i)
	{
		world->removeConstraint(constraints[i]);
		RELEASE(constraints[i]);
	}

	constraints.clear();
}

void ModulePhysics3D::ClearBalls()
{
	// Clear all the debug balls
	for (int i = 0; i < physBalls.size(); ++i)
		world->removeCollisionObject(physBalls[i]);

	for (int i = 0; i < balls.size(); ++i) {
		RELEASE(balls[i]);
		RELEASE(spheres[i]);
	}
	balls.clear();
	spheres.clear();
}

void ModulePhysics3D::PhysicalizeScene(GameObject* root)
{
	if (root->GetComponent<ComponentMesh>() != nullptr)
		AddRigidBody(root->globalOBB, root, 10.0f);

	for (int i = 0; i < root->children.size(); ++i)
		PhysicalizeScene(root->children[i]);
}

// Debug Drawer
//void ModulePhysics3D::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
//{
//	glLineWidth(2.0f);
//	glBegin(GL_LINES);
//
//	glColor3f(color.x(), color.y(), color.z());
//
//	glVertex3f(from.x(), from.y(), from.z());
//	glVertex3f(to.x(), to.y(), to.z());
//
//	glColor3f(255, 255, 255);
//	glEnd();
//}
//
//void ModulePhysics3D::reportErrorWarning(const char* warningString)
//{
//	LOG("Bullet warning: %s", warningString);
//}
//
//void ModulePhysics3D::draw3dText(const btVector3& location, const char* textString)
//{
//	LOG("Bullet draw text: %s", textString);
//}
//
//void ModulePhysics3D::setDebugMode(int debugMode)
//{
//	mode = (btIDebugDraw::DebugDrawModes)debugMode;
//}
//
//int	 ModulePhysics3D::getDebugMode() const
//{
//	return mode;
//}
