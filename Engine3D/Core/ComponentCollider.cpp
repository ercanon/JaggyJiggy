#include "Application.h"
#include "ModulePhysics3D.h"
#include "ComponentCollider.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#include "ImGui/imgui.h"

ComponentCollider::ComponentCollider(GameObject* parent, bool active) : Component(parent)
{
}

ComponentCollider::ComponentCollider(btRigidBody* body) : body(body)
{
	body->setUserPointer(this);
}

ComponentCollider::~ComponentCollider()
{
}

bool ComponentCollider::Update(float dt)
{
	if (owner != nullptr)
	{
		if (active == false)
		{
			mat4x4 bodyTransform, parentTransform, newTransform;
			body->getWorldTransform().getOpenGLMatrix(&bodyTransform);

			if (owner->parent != nullptr)
				parentTransform = owner->parent->transform->GetGlobalGLTransform();
			else
				parentTransform = IdentityMatrix;

			newTransform = parentTransform.inverse() * bodyTransform;

			owner->transform->SetLocalTransform(newTransform);
			owner->transform->Move(-localPosition);
		}
	}

	return true;
}

// Maybe need to clean "body". 

void ComponentCollider::OnGui()
{
	if (ImGui::CollapsingHeader("RigidBody"))
	{
		ImGui::Checkbox("Enabled", &active);
		if (ImGui::Button("Set to zero"))
		{
			localPosition = float3(0.0f, 0.0f, 0.0f);
			OnUpdateTransform();
		}

		if (ImGui::DragFloat3("Local Position", (float*)&localPosition, 0.1))
			OnUpdateTransform();

		if (ImGui::Checkbox("Dynamic object", &dynamicObject))
		{
			if (!dynamicObject)
			{
				App->physics->world->removeRigidBody(body);

				btVector3 localInertia(0, 0, 0);
				body->getCollisionShape()->calculateLocalInertia(0.0f, localInertia);
				body->setMassProps(0.0f, localInertia);

				App->physics->world->addRigidBody(body);
			}
			else
			{
				App->physics->world->removeRigidBody(body);

				btVector3 localInertia(0, 0, 0);
				body->getCollisionShape()->calculateLocalInertia(mass, localInertia);
				body->setMassProps(mass, localInertia);

				App->physics->world->addRigidBody(body);
			}
		}

		if (dynamicObject)
		{
			if (ImGui::DragFloat("Mass of the building: %.2f", &mass))
			{
				App->physics->world->removeRigidBody(body);

				btVector3 localInertia(0, 0, 0);
				body->getCollisionShape()->calculateLocalInertia(mass, localInertia);
				body->setMassProps(mass, localInertia);

				App->physics->world->addRigidBody(body);
			}
		}
	}
}

void ComponentCollider::CreateBody(btRigidBody* body)
{
	this->body = body;
	body->setUserPointer(this);
}

// Pushes the rigidbody with velocity determined by a vector
void ComponentCollider::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

// Gets the transform of the Rigidbody
void ComponentCollider::GetTransform(float* matrix) const
{
	if (body != NULL && matrix != NULL)
	{
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

// Sets the transform of the Rigidbody
void ComponentCollider::SetTransform(const float* matrix) const
{
	if (body != NULL && matrix != NULL)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

// Sets the position of the Rigidbody
void ComponentCollider::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
}

// Rotates the Rigidbody
void ComponentCollider::RotateBody(btQuaternion rotationQuaternion) {

	btTransform t = body->getWorldTransform();
	t.setRotation(rotationQuaternion);
	body->setWorldTransform(t);
}

void ComponentCollider::OnUpdateTransform()
{
	if (followObject)
	{
		float4x4 objectTransform = owner->transform->transformMatrix;
		objectTransform = objectTransform * objectTransform.Translate(localPosition);

		mat4x4 glTransform;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				glTransform.M[i * 4 + j] = objectTransform[j][i];

		btTransform newTransform;
		newTransform.setFromOpenGLMatrix(&glTransform);
		body->setWorldTransform(newTransform);
	}
}


