#include "Player.h"
#include"myEngine/Frame/Frame.h"
#include"application/Camera/FollowCamera.h"
#include"application/Enemy/Enemy.h"

void Player::Init()
{
	BaseObject::Init();
	BaseObject::CreateModel("debug/Cube.obj");
	transform_.translation_ += transform_.scale_.y;
	weapon_ = std::make_unique<Weapon>();
	weapon_->Init();
	weapon_->SetParent(transform_);
}

void Player::Update()
{
	Move();
	Rotation();
	BaseObject::Update();
	weapon_->Update();
}

void Player::Draw(const ViewProjection& viewProjection)
{
	BaseObject::Draw(viewProjection);
	weapon_->Draw(viewProjection);
}

void Player::DebugTransform(const std::string className)
{
	BaseObject::DebugTransform(className);
	weapon_->DebugTransform("ウェポン ");
}

void Player::imgui()
{
	if (ImGui::BeginTabItem("プレイヤー")) {
		ImGui::DragFloat("動く速度", &kMoveSpeed, 0.01f);
		ImGui::DragFloat("ダッシュ速度", &kDashSpeed, 0.01f);
		ImGui::DragFloat("ダッシュ減衰率", &kDashDecay, 0.01f);
		ImGui::EndTabItem();
	}
}

void Player::Move()
{
	const float diagonalSpeedFactor = 1.0f / sqrt(2.0f);
	// クールタイム処理
	if (dashCoolTime_ > 0.0f) {
		dashCoolTime_ -= Frame::DeltaTime(); // フレーム間隔を想定
	}

	// 移動ベクトルの初期化
	move = Vector3(0.0f, 0.0f, 0.0f);

	// カメラの向きに基づいて進行方向を決定
	if (camera_) {
		float cameraYaw = camera_->GetYaw(); // カメラのyawを取得

		// Wキーで進む方向を決定（カメラの向きに基づく）
		if (Input::GetInstance()->PushKey(DIK_W)) {
			move.x += std::sin(cameraYaw);  // カメラの向きに基づいてX軸方向に進む
			move.z += std::cos(cameraYaw);  // カメラの向きに基づいてZ軸方向に進む
		}
		// Sキーで後退
		if (Input::GetInstance()->PushKey(DIK_S)) {
			move.x -= std::sin(cameraYaw);  // カメラの向きに基づいてX軸方向に後退
			move.z -= std::cos(cameraYaw);  // カメラの向きに基づいてZ軸方向に後退
		}

		// Aキーで左移動（カメラの向きに基づく）
		if (Input::GetInstance()->PushKey(DIK_A)) {
			move.x -= std::cos(cameraYaw);  // カメラの向きに基づいて左移動
			move.z += std::sin(cameraYaw);  // カメラの向きに基づいてZ軸方向に進む
		}

		// Dキーで右移動（カメラの向きに基づく）
		if (Input::GetInstance()->PushKey(DIK_D)) {
			move.x += std::cos(cameraYaw);  // カメラの向きに基づいて右移動
			move.z -= std::sin(cameraYaw);  // カメラの向きに基づいてZ軸方向に進む
		}
	}

	// ダッシュ処理
	if (Input::GetInstance()->TriggerKey(DIK_LSHIFT) && dashCoolTime_ <= 0.0f) {
		dashSpeed_ = kDashSpeed;
		dashCoolTime_ = 1.0f; // クールタイム設定
	}

	// 徐々にダッシュ速度を戻す
	dashSpeed_ *= kDashDecay;
	if (dashSpeed_ < kMoveSpeed) {
		dashSpeed_ = kMoveSpeed;
	}

	// 移動方向ベクトルがゼロでない場合に処理
	if (move.x != 0.0f || move.z != 0.0f) {
		if (move.x != 0.0f && move.z != 0.0f) {
			move.x *= diagonalSpeedFactor;
			move.z *= diagonalSpeedFactor;
		}
		transform_.translation_ += move * dashSpeed_;  // ダッシュ速度を適用
	}
}


void Player::Rotation()
{
	// 移動ベクトルがゼロでない場合の回転処理
	if (move.x != 0.0f || move.z != 0.0f) {
		float targetRotation = std::atan2(move.x, move.z); // 進行方向の角度計算
		const float rotationSpeed = 0.1f; // 補間速度

		// 現在の回転角度を -π から π の範囲に正規化
		while (transform_.rotation_.y - targetRotation > std::numbers::pi_v<float>) {
			transform_.rotation_.y -= 2.0f * std::numbers::pi_v<float>;
		}
		while (transform_.rotation_.y - targetRotation < -std::numbers::pi_v<float>) {
			transform_.rotation_.y += 2.0f * std::numbers::pi_v<float>;
		}

		// スムーズな回転のための線形補間
		transform_.rotation_.y = transform_.rotation_.y + (targetRotation - transform_.rotation_.y) * rotationSpeed;
	}
}

Vector3 Player::GetCenterPosition() const
{
	return transform_.translation_;
}

Vector3 Player::GetCenterRotation() const
{
	return transform_.rotation_;
}

void Player::OnCollision(Collider* other)
{
	if (dynamic_cast<Enemy*>(other)) {

	}
}
