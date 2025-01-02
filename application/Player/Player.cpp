#define NOMINMAX
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

	afterImageEmitter_ = std::make_unique<ParticleEmitter>();
	afterImageEmitter_->Initialize("afterImage", "debug/Cube.obj");
}

void Player::Update()
{
	Move();
	Rotation();
	AffterEffect();
	BaseObject::Update();
	weapon_->Update();
}

void Player::Draw(const ViewProjection& viewProjection)
{
	BaseObject::Draw(viewProjection);
	weapon_->Draw(viewProjection);
}

void Player::DrawParticle(const ViewProjection& viewProjection)
{
	afterImageEmitter_->Update(viewProjection);
	afterImageEmitter_->Draw();
}

void Player::DebugTransform(const std::string className)
{
	BaseObject::DebugTransform(className);
	weapon_->DebugTransform("ウェポン ");
}

void Player::imgui()
{
	if (ImGui::BeginTabBar("player")) {
		if (ImGui::BeginTabItem("プレイヤー")) {
			ImGui::DragFloat3("速度", &speed.x, 0.01f);
			ImGui::DragFloat("動く速度", &kMoveSpeed, 0.01f);
			ImGui::DragFloat("ダッシュ速度", &kDashSpeed, 0.01f);
			ImGui::DragFloat("ダッシュ減衰率", &kDashDecay, 0.01f);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	afterImageEmitter_->imgui();
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

	if (!Input::GetInstance()->PushKey(DIK_D) && !Input::GetInstance()->PushKey(DIK_W) && !Input::GetInstance()->PushKey(DIK_A) && !Input::GetInstance()->PushKey(DIK_S)) {
		speed = { 0.0f,0.0f,0.0f };
		move = { 0.0f,0.0f,0.0f };
		dashSpeed_ = 0.0f;
	}

	// ダッシュ処理
	if ((Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_W) || Input::GetInstance()->PushKey(DIK_A) || Input::GetInstance()->PushKey(DIK_S)) && Input::GetInstance()->TriggerKey(DIK_LSHIFT) && dashCoolTime_ <= 0.0f) {
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
		speed = move * dashSpeed_;
		transform_.translation_ += speed;  // ダッシュ速度を適用
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

void Player::AffterEffect()
{
	afterImageEmitter_->SetPosition(transform_.translation_);

	// 最小と最大のfrequencyの値をローカル変数で設定
	const float minFrequency = 0.045f;
	const float maxFrequency = 0.15f;

	// speedが0.3fより大きいまたは-0.3fより小さい場合にパーティクルの間隔を設定
	if (speed.x > 0.3f || speed.y > 0.3f || speed.z > 0.3f ||
		speed.x < -0.3f || speed.y < -0.3f || speed.z < -0.3f)
	{
		// speedの大きさに応じて間隔を設定
		float speedMagnitude = std::max({ std::abs(speed.x), std::abs(speed.y), std::abs(speed.z) });

		// 最小間隔と最大間隔をスケーリング
		float newFrequency = maxFrequency - (speedMagnitude - 0.3f) * (maxFrequency - minFrequency) / (1.0f - 0.3f);
		newFrequency = std::clamp(newFrequency, minFrequency, maxFrequency);  // 間隔がminFrequencyからmaxFrequencyに収束するようにクランプ

		afterImageEmitter_->SetFrequency(newFrequency);
		afterImageEmitter_->SetCount(1);  // 速度が速い場合にエフェクトを表示
		afterImageEmitter_->SetStartRotate(transform_.rotation_);
		afterImageEmitter_->SetEndRotate(transform_.rotation_);
	}
	else
	{
		afterImageEmitter_->SetCount(0);  // 速度が遅い場合はエフェクトを非表示
	}
}

Vector3 Player::GetCenterPosition() const
{
	return BaseObject::GetCenterPosition();
}

Vector3 Player::GetCenterRotation() const
{
	return BaseObject::GetCenterRotation();
}

void Player::OnCollision(Collider* other)
{
	if (dynamic_cast<Enemy*>(other)) {

	}
}
