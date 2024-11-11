#include "PlayerBullet.h"
#include <TextureManager.h>
#include"enemy.h"

void playerBullet::Initialize( const Vector3& position, const Vector3& velocity) {

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("ICO.obj");

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
