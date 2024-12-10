#include "FollowCamera.h"

void FollowCamera::Init() {
	viewProjection_.farZ = 1100;
	viewProjection_.Initialize();
	worldTransform_.Initialize();
}

void FollowCamera::Update() {
	// 追従対象がいれば
	if (target_) {
		// 目標位置を設定（親オブジェクトの位置を基に設定）
		Vector3 targetPosition = target_->translation_ + Vector3(0.0f, 5.0f, -25.0f);

		// カメラの現在位置と目標位置を補間
		// 補間のスピードを調整するための変数（遅延具合を調整）
		float interpolationSpeed = 0.1f; // 値を調整して遅延具合を変える
		worldTransform_.translation_ = worldTransform_.translation_ + (targetPosition - worldTransform_.translation_) * interpolationSpeed;

		// 親子関係の更新
		worldTransform_.UpdateMatrix();
	}

	// ビュー行列にカメラの位置を反映
	viewProjection_.translation_ = worldTransform_.translation_;
	viewProjection_.rotation_ = worldTransform_.rotation_;
	viewProjection_.matWorld_ = worldTransform_.matWorld_;

	// ビュー行列の更新
	viewProjection_.UpdateMatrix();
}

void FollowCamera::imgui()
{
	ImGui::Begin("FollowCamera");
	ImGui::DragFloat3("wt position", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("vp position", &viewProjection_.translation_.x, 0.1f);
	ImGui::End();
}
