#include "Ground.h"

void Ground::Init()
{
	BaseObject::Init();
	BaseObject::CreateModel("debug/Ground.obj");
}

void Ground::Update()
{
	BaseObject::Update();
}

void Ground::Draw(const ViewProjection& viewProjection)
{
	BaseObject::Draw(viewProjection);
}

void Ground::DebugTransform(const std::string className)
{
	BaseObject::DebugTransform(className);
}
