#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "glew.h"
#include "ImGui/imgui.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent) {
	
	position = float3::zero;
	rotation = Quat::identity;
	scale = float3::one;

	transformMatrix.SetIdentity();
	transformMatrixLocal.SetIdentity();
}


bool ComponentTransform::Update(float dt) {
	if (isDirty)
	{
		transformMatrixLocal = float4x4::FromTRS(position, rotation, scale);
		RecomputeGlobalMatrix();
		owner->PropagateTransform();
		isDirty = false;
	}
	return true;
}

void ComponentTransform::OnGui()
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		float3 newPosition = position;
		if (ImGui::DragFloat3("Location", &newPosition[0]))
		{
			if (App->editor->pause == true)
				SetPosition(newPosition);
		}
		float3 newRotationEuler;
		newRotationEuler.x = RADTODEG * rotationEuler.x;
		newRotationEuler.y = RADTODEG * rotationEuler.y;
		newRotationEuler.z = RADTODEG * rotationEuler.z;
		if (ImGui::DragFloat3("Rotation", &(newRotationEuler[0])))
		{
			if (App->editor->pause == true)
			{
				newRotationEuler.x = DEGTORAD * newRotationEuler.x;
				newRotationEuler.y = DEGTORAD * newRotationEuler.y;
				newRotationEuler.z = DEGTORAD * newRotationEuler.z;
				SetRotation(newRotationEuler);
			}
		}
		float3 newScale = scale;
		if (ImGui::DragFloat3("Scale", &(newScale[0])))
		{
			if (App->editor->pause == true)
			SetScale(newScale);
		}
	}
}

void ComponentTransform::Move(float3 positionIncrease)
{
	position += positionIncrease;
}

void ComponentTransform::SetPosition(const float3& newPosition)
{
	position = newPosition;
	isDirty = true;
}

void ComponentTransform::SetRotation(const float3& newRotation)
{
	rotation = Quat::FromEulerXYZ(newRotation.x, newRotation.y, newRotation.z);
	rotationEuler = newRotation;
	isDirty = true;
}

void ComponentTransform::SetScale(const float3& newScale)
{
	scale = newScale;
	isDirty = true;
}

void ComponentTransform::SetLocalTransform(const mat4x4& transform)
{
	float4x4 newTransform;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			newTransform[j][i] = transform.M[i * 4 + j];

	newTransform.Decompose(position, rotation, scale);
	rotationEuler = rotation.ToEulerXYZ().Abs();
}

void ComponentTransform::NewAttachment()
{
	if (owner->parent != App->scene->root)
		transformMatrixLocal = owner->parent->transform->transformMatrix.Inverted().Mul(transformMatrix);

	float3x3 rot;
	transformMatrixLocal.Decompose(position, rot, scale);
	rotationEuler = rot.ToEulerXYZ();
}

void ComponentTransform::OnParentMoved()
{
	RecomputeGlobalMatrix();
}

void ComponentTransform::RecomputeGlobalMatrix()
{
	if (owner->parent != nullptr)
	{
		transformMatrix = owner->parent->transform->transformMatrix.Mul(transformMatrixLocal);
	}
	else
	{
		transformMatrix = transformMatrixLocal;
	}

	//Update Globals Bounding Boxes
	if (owner->GetComponent<ComponentMesh>() != nullptr)
		owner->GetComponent<ComponentMesh>()->GenerateBounds(false);
}

const mat4x4 ComponentTransform::GetGlobalGLTransform() const
{
	mat4x4 ret;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			ret.M[i * 4 + j] = transformMatrix[j][i];

	return ret;
}