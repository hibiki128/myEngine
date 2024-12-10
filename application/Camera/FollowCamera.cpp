#include "FollowCamera.h"
#include "Input.h"

void FollowCamera::Init() {
	viewProjection_.farZ = 1100;
	viewProjection_.Initialize();
	worldTransform_.Initialize();
}

void FollowCamera::Update() {
	// 追従対象がいれば
	if (target_) {
		worldTransform_.parent_ = target_;
		worldTransform_.translation_ = { 0.0f,10.0f,-25.0f };
	}
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
