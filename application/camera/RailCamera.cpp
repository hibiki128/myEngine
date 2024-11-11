#include "RailCamera.h"
#include <imgui.h>

RailCamera::RailCamera() : velocity_(0.000f) {} // 初期速度を設定

RailCamera::~RailCamera() {}

void RailCamera::Initialize(WorldTransform& worldTransform) {
	// ワールドトランスフォームの初期化
	worldTransform_.translation_.x = worldTransform.matWorld_.m[3][0];
	worldTransform_.translation_.y = worldTransform.matWorld_.m[3][1];
	worldTransform_.translation_.z = worldTransform.matWorld_.m[3][2];
	worldTransform_.rotation_ = worldTransform.rotation_;
	worldTransform_.translation_.z = -50.0f;

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 1100;
	viewProjection_.Initialize();
}

void RailCamera::Update() {
	// tの更新。velocityに依存してカメラを前進させる
	t_ += velocity_;
	if (t_ > 1.0f) t_ = 1.0f; // tが1.0を超えないように制限

	// カーブパス上の位置を計算
	Vector3 eye = CatmullRomPosition(controlPoints_, t_);
	eye.y += 2.0f;
	eye.z += 1.0f;

	// プレイヤーの位置に依存した目標位置を計算する
	float t2 = t_ + 0.01f; // tより少し先の位置を計算
	if (t2 > 1.0f) t2 = 1.0f; // t2が1.0を超えないように制限
	Vector3 target = CatmullRomPosition(controlPoints_, t2);

	target.y += 2.0f;
	target.z += 1.0f;

	// カメラ位置と方向の設定
	worldTransform_.translation_ = eye;
	Vector3 forward = (target - eye).Normalize();

	// カメラのZ軸周りの回転角度を計算
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(-worldTransform_.rotation_.z);
	Vector3 forwardZ = TransformNormal(forward, rotateZMatrix);

	// カメラのY軸周りの回転角度を計算
	worldTransform_.rotation_.y = std::atan2(forwardZ.x, forwardZ.z);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(-worldTransform_.rotation_.y);
	forwardZ = TransformNormal(forwardZ, rotateYMatrix);

	// カメラのX軸周りの回転角度を計算
	worldTransform_.rotation_.x = std::atan2(-forwardZ.y, forwardZ.z);

	// ワールド行列を更新
	worldTransform_.UpdateWorld();

	// ビュー行列の計算
	viewProjection_.matView_ = Inverse(worldTransform_.matWorld_);
}

void RailCamera::imgui() {
#ifdef _DEBUG
	if (ImGui::BeginTabItem("WorldTransform")) {
		ImGui::SliderFloat3("Translation", &worldTransform_.translation_.x, -50.0f, 1000.0f);
		ImGui::SliderFloat3("Rotate", &worldTransform_.rotation_.x, 0, 10.0f);
		ImGui::SliderFloat("Velocity", &velocity_, 0.01f, 1.0f); // 速度を調整するためのスライダー
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("ViewProjection")) {
		ImGui::SliderFloat3("Translation", &viewProjection_.translation_.x, -50.0f, 1000.0f);
		ImGui::SliderFloat3("Rotate", &viewProjection_.rotation_.x, 0, 10.0f);
		ImGui::EndTabItem();
	}
#endif
}