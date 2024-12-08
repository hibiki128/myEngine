#include "Ground.h"

void Ground::Initialize()
{
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("game/ground.obj");
	wt_.Initialize();
}

void Ground::Update()
{
	wt_.UpdateMatrix();
}

void Ground::Draw(const ViewProjection& viewProjection)
{
	obj_->Draw(wt_, viewProjection);
}

Vector3 Ground::GetCenterPosition() const
{
	const Vector3 offset = { 0.0f, 1.0f, 0.0f };//ローカル座標のオフセット
	// ワールド座標に変換
	Vector3 worldPos = Transformation(offset, wt_.matWorld_);

	return worldPos;
}

Vector3 Ground::GetCenterRotation() const
{
	return Vector3();
}
