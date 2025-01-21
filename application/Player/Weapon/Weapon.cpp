#include "Weapon.h"

void Weapon::Init(const std::string className)
{
	BaseObject::Init(className);
	BaseObject::CreateModel("Player/Weapon/Weapon.obj");
	BaseObject::CreateCollider();
	transform_.translation_ = { 0.0f,0.2f,0.0f };
	afterImageEmitter_ = std::make_unique<ParticleEmitter>();
	afterImageEmitter_->Initialize("afterImageWeapon", "Player/Weapon/Weapon.obj");

}

void Weapon::Update()
{
	afterImageEmitter_->imgui();
	BaseObject::Update();
}

void Weapon::Draw(const ViewProjection& viewProjection)
{
	BaseObject::Draw(viewProjection);
}

void Weapon::DrawParticle()
{
	afterImageEmitter_->Draw();
}

void Weapon::Debug()
{
	BaseObject::DebugImGui();
}

void Weapon::ParticleUpdate(const ViewProjection& viewProjection)
{
	afterImageEmitter_->Update(viewProjection);
	afterImageEmitter_->SetPosition(GetCenterPosition());
	afterImageEmitter_->SetStartRotate(GetCenterRotation());
	afterImageEmitter_->SetEndRotate(GetCenterRotation());

}

// 子オブジェクトのワールド座標を計算する
Vector3 Weapon::GetCenterPosition() const
{
	const Vector3 offSet = { 0.0f,0.0f,0.0f };
	Vector3 worldPos = Transformation(offSet, transform_.matWorld_);
	return worldPos;
}

// 子オブジェクトの回転を親の回転を考慮して計算する
Vector3 Weapon::GetCenterRotation() const
{
	// 親の位置に対する相対的な位置をローカル位置として計算
	Vector3 worldRotation = transform_.rotation_;

	// 親がいる場合、その位置と回転を考慮する
	if (transform_.parent_ != nullptr)
	{
		// 親の回転を考慮して相対位置をワールド座標に変換
		worldRotation = transform_.parent_->rotation_;
	}

	return worldRotation;
}
