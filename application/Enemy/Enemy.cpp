#include "Enemy.h"
#include"application/Player/Player.h"
#include <myEngine/Frame/Frame.h>
#include"application/Player/Weapon/Weapon.h"

void Enemy::Init()
{
	BaseObject::Init();
	BaseObject::CreateModel("debug/Cube.obj");
	objColor_.SetColor({ 1.0f,0.0f,0.0f,1.0f });
	transform_.translation_ += transform_.scale_.y;
	HP = 10;
}

void Enemy::Update()
{
	if (!isDead_) {
		if (behaviorRequest_) {
			// 振るまいを変更する
			behavior_ = behaviorRequest_.value();
			// 各振るまいごとの初期化を実行
			switch (behavior_) {
			case Behavior::kRoot:
			default:
				BehaviorRootInitialize();
				break;
			case Behavior::kKnockback:
				BehaviorKnockbackInitialize();
				break;
			}
			// 振るまいリクエストをリセット
			behaviorRequest_ = std::nullopt;
		}
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootUpdate();
			break;
		case Behavior::kKnockback:
			BehaviorKnockbackInitialize();
			break;
		}

		BaseObject::Update();
	}
	else {
		Collider::SetCollisionEnabled(false);
	}
	if (HP <= 0) {
		isDead_ = true;
	}
}

void Enemy::Draw(const ViewProjection& viewProjection)
{
	if (!isDead_) {
	
		BaseObject::Draw(viewProjection);
	}
}

void Enemy::DebugTransform(const std::string className)
{
	BaseObject::DebugTransform(className);
}

void Enemy::Move()
{
	// プレイヤーとの位置ベクトルを計算
	Vector3 playerPosition = player_->GetPosition();  // プレイヤーの位置
	Vector3 enemyPosition = transform_.translation_; // 敵の位置
	Vector3 direction = playerPosition - enemyPosition; // プレイヤーと敵の位置差

	// ベクトルの正規化（単位ベクトルに変換）
	direction.Normalize();

	// 移動スピードを掛け算
	float speed = 0.005f;
	Vector3 movement = direction * speed;

	// 敵の位置を移動させる
	transform_.translation_.x += movement.x;
	transform_.translation_.z += movement.z;
}


void Enemy::BehaviorRootInitialize()
{
}

void Enemy::BehaviorKnockbackInitialize()
{
}

void Enemy::BehaviorRootUpdate()
{
	Move();
}

void Enemy::BehaviorKnockbackUpdate()
{
}

Vector3 Enemy::GetCenterPosition() const
{
	return BaseObject::GetCenterPosition();
}

Vector3 Enemy::GetCenterRotation() const
{
	return BaseObject::GetCenterRotation();
}

void Enemy::OnCollision(Collider* other)
{
	if (dynamic_cast<Weapon*>(other)) {

	}
}

void Enemy::OnCollisionEnter(Collider* other)
{
	if (HP > 0) {
		if (dynamic_cast<Weapon*>(other)) {
			--HP;
		}
	}
}
