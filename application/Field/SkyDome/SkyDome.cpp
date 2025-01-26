#include "SkyDome.h"

void SkyDome::Init(const std::string className)
{
	BaseObject::Init(className);
	BaseObject::CreateModel("Field/skyDome.obj");
	Collider::SetCollisionEnabled(false);
	transform_.scale_ = { 10.0f,10.0f,10.0f };
}

void SkyDome::Update()
{
	BaseObject::Update();
}

void SkyDome::Draw(const ViewProjection& viewProjection)
{
	BaseObject::Draw(viewProjection);
}
