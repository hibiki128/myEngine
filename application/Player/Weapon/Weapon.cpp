#include "Weapon.h"

void Weapon::Init()
{
	BaseObject::Init();
	BaseObject::CreateModel("debug/ICO.obj");
}

void Weapon::Update()
{
	BaseObject::Update();
}

void Weapon::Draw(const ViewProjection& viewProjection)
{
	BaseObject::Draw(viewProjection);
}

void Weapon::DebugTransform(const std::string className)
{
	BaseObject::DebugTransform(className);
}

// 子オブジェクトのワールド座標を計算する
Vector3 Weapon::GetCenterPosition() const
{
	// 親の位置に対する相対的な位置をローカル位置として計算
	Vector3 worldPosition = transform_.translation_;

	// 親がいる場合、その位置と回転を考慮する
	if (transform_.parent_ != nullptr)
	{
		// 親の回転を考慮して相対位置をワールド座標に変換
		worldPosition = Transformation(worldPosition, transform_.parent_->matWorld_);
	}

	return worldPosition;
}

// 子オブジェクトの回転を親の回転を考慮して計算する
Vector3 Weapon::GetCenterRotation() const
{
	Vector3 worldRotation = transform_.rotation_;

	if (transform_.parent_ != nullptr)
	{
		// 親の回転を考慮して子のローカル回転をワールド回転に変換
		worldRotation += transform_.parent_->rotation_;
	}

	return worldRotation;
}