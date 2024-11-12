#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include"myMath.h"
class RailCamera {
private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	float time_ = 0.0f;

	std::vector<Vector3> controlPoints_;

	float velocity_ = 0;

	float t_ = 0;

	bool isFinish = false;

public:

	RailCamera();
	~RailCamera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WorldTransform& worldTransform);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void imgui();

	/// <summary>
	/// ゲッター
	/// </summary>
	/// <returns></returns>
	const ViewProjection& GetViewProjection() const { return viewProjection_; }

	const WorldTransform& GetWorldTransform() const { return worldTransform_; };

	void SetControlPoints(std::vector<Vector3> controlPoints) { controlPoints_ = controlPoints; }
	bool IsFinish() { return isFinish; }
};
