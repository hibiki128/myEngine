#pragma once
#include"object3d.h"
#include"WorldTransform.h"
#include"math/Vector3.h"
#include"Collider.h"
class playerBullet :public Collider{
private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	std::unique_ptr<Object3d> obj_ = nullptr;

	// 速度
	Vector3 velocity_;

	// 寿命<frm>
	static const int32_t kLifeTime = 10 * 10;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	static inline const float kRadius_ = 1.0f;

	void OnCollisionEnter([[maybe_unused]] Collider* other) override;

	Vector3 GetCenterPosition()const override;

	AABB GetAABB()const override;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	/// <param name="position"></param>
	void Initialize(const Vector3& position, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProejection"></param>
	void Draw(const ViewProjection& viewProejection);

	/// <summary>
	/// 衝突判定
	/// </summary>
	void OnCollision();

	/// <summary>
	/// ゲッター
	/// </summary>
	/// <returns></returns>
	bool IsDead() const { return isDead_; }
	float GetRadius() { return kRadius_; }
	Vector3 GetWorldPosition();
};
