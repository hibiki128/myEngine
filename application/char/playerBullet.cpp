#include "PlayerBullet.h"
#include <TextureManager.h>
#include"enemy.h"

void playerBullet::OnCollisionEnter(Collider* other)
{

}

Vector3 playerBullet::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変更
	Vector3 worldPos = Transformation(offset, worldTransform_.matWorld_);
	return worldPos;
}

AABB playerBullet::GetAABB() const
{
	// 中心位置を取得
	Vector3 center = GetCenterPosition();
	// スケール値を半分にしてAABBの範囲とする
	Vector3 halfScale = worldTransform_.scale_ * 1.1f;

	// min と max の計算
	AABB aabb;
	aabb.min = center - halfScale;
	aabb.max = center + halfScale;

	return aabb;
}

void playerBullet::Initialize(const Vector3& position) {

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("debug/Cube.obj");

	worldTransform_.Initialize();
	// 引数で受け取った座標をセット
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = { 0.3f,0.3f,5.0f };
}

void playerBullet::Update() {
	// 座標を移動させる(1フレーム文の移動量を足しこむ)
	//worldTransform_.translation_ += velocity_;

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
