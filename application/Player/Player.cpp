#include "Player.h"

void Player::Init()
{
	BaseObject::Init();
	BaseObject::CreateModel("debug/suzannu.obj");
	transform_.translation_ += transform_.scale_.y;
}

void Player::Update()
{
	Move();
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

void Player::Move()
{
	const float kMoveSpeed = 0.3f;
	const float diagonalSpeedFactor = 1.0f / sqrt(2.0f);  // 斜め方向補正係数（√2で割る）

	// x と z の移動方向を格納するベクトル
	Vector3 move(0.0f, 0.0f, 0.0f);

	// W/Sキーでの前後移動
	if (Input::GetInstance()->PushKey(DIK_W)) {
		move.z += 1.0f;  // Wキーで前進
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {
		move.z -= 1.0f;  // Sキーで後退
	}

	// A/Dキーでの左右移動
	if (Input::GetInstance()->PushKey(DIK_A)) {
		move.x -= 1.0f;  // Aキーで左移動
	}
	if (Input::GetInstance()->PushKey(DIK_D)) {
		move.x += 1.0f;  // Dキーで右移動
	}

	// 移動方向ベクトルがゼロでない場合に処理
	if (move.x != 0.0f || move.z != 0.0f) {
		// 斜め移動時の補正（xとzを正規化して、一定の速度にする）
		if (move.x != 0.0f && move.z != 0.0f) {
			move.x *= diagonalSpeedFactor;  // 斜め移動時は補正を掛ける
			move.z *= diagonalSpeedFactor;
		}
	}

	// 正規化後、速度を掛ける
	transform_.translation_ += move * kMoveSpeed;  // 一定速度で移動
}

Vector3 Player::GetCenterPosition() const
{
	return transform_.translation_;
}

Vector3 Player::GetCenterRotation() const
{
	return transform_.rotation_;
}
