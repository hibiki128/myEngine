#include "Enemy.h"

void Enemy::Init()
{
	BaseObject::Init();
	BaseObject::CreateModel("debug/sphere.obj");
	transform_.translation_ += transform_.scale_.y;
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

Vector3 Enemy::GetCenterPosition() const
{
	return BaseObject::GetCenterPosition();
}

Vector3 Enemy::GetCenterRotation() const
{
	return BaseObject::GetCenterRotation();
}
