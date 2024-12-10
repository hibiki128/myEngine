#include "Player.h"

void Player::Init()
{
	BaseObject::Init();
	BaseObject::CreateModel("debug/suzannu.obj");
}

void Player::Update()
{
	BaseObject::Update();
}

void Player::Draw(const ViewProjection& viewProjection)
{
	BaseObject::Draw(viewProjection);
}

void Player::DebugTransform(const std::string className)
{
	BaseObject::DebugTransform(className);
}

//Vector3 Player::GetCenterPosition() const
//{
//	return transform_.translation_;
//}
//
//Vector3 Player::GetCenterRotation() const
//{
//	return transform_.rotation_;
//}
