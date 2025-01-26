#include "Ground.h"

void Ground::Init(const std::string className)
{
	BaseObject::Init(className);
	BaseObject::CreateModel("Field/Ground.obj");
	Collider::SetCollisionEnabled(false);
}

void Ground::Update()
{
	BaseObject::Update();
}

void Ground::Draw(const ViewProjection& viewProjection)
{
	BaseObject::Draw(viewProjection);
}
