#include "PlayerBullet.h"

void PlayerBullet::Initialize(const Vector3& position, const Vector3& velocity)
{
	obj3d_ = std::make_unique<Object3d>();
	obj3d_->Initialize("PlayerBullet.obj");
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.3f,0.3f ,0.3f };
	velocity_ = velocity;

	Collider::SetAtrribute("PlayerBullet");
	Collider::SetMask({ "Player","PlayerBullet"});

	boundingBox_ = BoundingBox::Sphere_3D;
	radius_ = 0.5f;

}

void PlayerBullet::Update(const Vector3& center)
{
	center_ = center;

	// 移動速度(velocity_)を適用して移動します
	worldTransform_.translation_ += velocity_;

	// 中心点(center)からの距離を計算します
	Vector3 distance = worldTransform_.translation_ - center_;

	// 0.2fの立方体範囲内にいるかチェック
	if (--deathTimer_ <= 0
		/*\\(std::abs(distance.x) <= 0.4f && std::abs(distance.y) <= 0.4f && std::abs(distance.z) <= 0.4f)*/)
	{
 		isDead_ = true;
	}

	worldTransform_.UpdateMatrix();  // マトリックスを更新します
}


void PlayerBullet::Draw(const ViewProjection& viewProejection)
{
	if (!isDead_) {
		obj3d_->Draw(worldTransform_, viewProejection);
	}
}

Vector3 PlayerBullet::GetWorldPosition()
{
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void PlayerBullet::OnCollision()
{
	isDead_ = true;
}
