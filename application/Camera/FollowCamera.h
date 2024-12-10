#pragma once
#include"WorldTransform.h"
#include "ViewProjection.h"
class FollowCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void imgui();

	/// <summary>
	/// ターゲット設定
	/// </summary>
	/// <param name="target"></param>
	void SetTarget(const WorldTransform* target) { target_ = target; }
	const ViewProjection& GetViewProjection() const { return viewProjection_; }

private:
	// ビュープロジェクション
	ViewProjection viewProjection_;

	WorldTransform worldTransform_;
	// 追従対象
	const WorldTransform* target_ = nullptr;
};
