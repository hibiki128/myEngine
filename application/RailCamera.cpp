#include "RailCamera.h"
#include <imgui.h>

RailCamera::RailCamera() {}

RailCamera::~RailCamera() {}

void RailCamera::Initialize(WorldTransform& worldTransform) {
	// ワールドトランスフォーム
	worldTransform_.translation_.x = worldTransform.matWorld_.m[3][0];
	worldTransform_.translation_.y = worldTransform.matWorld_.m[3][1];
	worldTransform_.translation_.z = worldTransform.matWorld_.m[3][2];
	worldTransform_.rotation_ = worldTransform.rotation_;
	worldTransform_.translation_.z = -50.0f;
	// ビュープロジェクション
	viewProjection_.farZ = 1100;
	viewProjection_.Initialize();
}

void RailCamera::Update() {

	const float LimitTime = 1500.0f;
	if (time_ < LimitTime) {
		time_++;
	}

	// カーブパス上の位置を計算するためのパラメータtを求める
	float t = 1.0f / LimitTime * time_;
	Vector3 eye = CatmullRomPosition(controlPoints_, t);

	// プレイヤーの位置に依存した目標位置を計算するためのパラメータt2を求める
	float t2 = 1.0f / LimitTime * (time_ + playerPosZ_);
	Vector3 target = CatmullRomPosition(controlPoints_, t2);

	// カメラの位置を設定する
	worldTransform_.translation_ = eye;

	// カメラの正面方向を計算する
	Vector3 forward = (target - eye).Normalize();

	// カメラのZ軸周りの回転角度を計算する
	worldTransform_.rotation_.z = std::atan2(-forward.y, forward.x);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(-worldTransform_.rotation_.z);
	Vector3 forwardZ = TransformNormal(forward, rotateZMatrix);

	// カメラのY軸周りの回転角度を計算する
	worldTransform_.rotation_.y = std::atan2(forwardZ.x, forwardZ.z);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(-worldTransform_.rotation_.y);
	forwardZ = TransformNormal(forwardZ, rotateYMatrix);

	// カメラのX軸周りの回転角度を計算する
	worldTransform_.rotation_.x = std::atan2(-forwardZ.y, forwardZ.z);

	// ワールド行列を更新する
	worldTransform_.UpdateWorld();

	// ビュー行列を計算するために、ワールド行列の逆行列を求める
	viewProjection_.matView_ = Inverse(worldTransform_.matWorld_);

#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::SliderFloat3("Translation", &worldTransform_.translation_.x, -50.0f, 1000.0f);
	ImGui::SliderFloat3("Rotate", &worldTransform_.rotation_.x, 0, 10.0f);
	ImGui::End();
#endif
}