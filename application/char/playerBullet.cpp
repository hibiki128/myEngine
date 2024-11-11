#include "PlayerBullet.h"
#include <TextureManager.h>
#include"enemy.h"

void playerBullet::OnCollisionEnter(Collider* other)
{

}

Vector3 playerBullet::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 40.0f };
	// ワールド座標に変更
	Vector3 worldPos = Transformation(offset, worldTransform_.matWorld_);
	return worldPos;
}

Vector3 playerBullet::GetCenterRotation() const
{
	return worldTransform_.rotation_;
}

AABB playerBullet::GetAABB() const
{
	// 中心位置を取得
	Vector3 center = GetCenterPosition();
	// スケール値を半分にしてAABBの範囲とする
	Vector3 halfScale = worldTransform_.scale_ * 0.7f;

	// min と max の計算
	AABB aabb;
	aabb.min = center - halfScale;
	aabb.max = center + halfScale;

	return aabb;
}

void playerBullet::Initialize(const Vector3& position) {

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("player/beam.obj");

	worldTransform_.Initialize();
	// 引数で受け取った座標をセット
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
}

void playerBullet::Update() {
	if (isFire_) {
		Collider::SetCollisionEnabled(true);
	}
	else {
		Collider::SetCollisionEnabled(false);
	}
	worldTransform_.UpdateMatrix();
}

void playerBullet::Draw(const ViewProjection& viewProjection) {
	if (isFire_) {
		obj_->Draw(worldTransform_, viewProjection);
	}
}

Vector3 playerBullet::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void playerBullet::SetParent(const WorldTransform* parent)
{
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}
