#include "Enemy.h"

void Enemy::Init()
{
	BaseObject::Init();
	BaseObject::CreateModel("debug/sphere.obj");
}

void Enemy::Update()
{
	BaseObject::Update();
}

void Enemy::Draw(const ViewProjection& viewProjection)
{
	BaseObject::Draw(viewProjection);
}

void Enemy::DebugTransform(const std::string className)
{
	BaseObject::DebugTransform(className);
}
