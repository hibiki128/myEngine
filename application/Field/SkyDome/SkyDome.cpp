#include "SkyDome.h"

void SkyDome::Init()
{
	BaseObject::Init();
	BaseObject::CreateModel("debug/skyDome.obj");
	Collider::SetCollisionEnabled(false);
}

void SkyDome::Update()
{
	BaseObject::Update();
}

void SkyDome::Draw(const ViewProjection& viewProjection)
{
	BaseObject::Draw(viewProjection);
}

void SkyDome::DebugTransform(const std::string className)
{
	BaseObject::DebugTransform(className);
}
