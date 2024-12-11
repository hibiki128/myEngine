#pragma once
#include"WorldTransform.h"
#include "ViewProjection.h"
class FollowCamera {
public:
	/// ===================================================
	///public method
	/// ===================================================
	void Init();

	void Update();

	void imgui();

	float GetYaw() { return yaw_; }

	void SetTarget(const WorldTransform* target) { target_ = target; }
	const ViewProjection& GetViewProjection() const { return viewProjection_; }

private:
	/// ===================================================
	///private method
	/// ===================================================

	void Move();

private:
	// ビュープロジェクション
	ViewProjection viewProjection_;

	WorldTransform worldTransform_;
	// 追従対象
	const WorldTransform* target_ = nullptr;

	float yaw_;
	float distanceFromTarget_;
	float heightOffset_;
};
