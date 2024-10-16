#pragma once
#include"Object3d.h"
#include"WorldTransform.h"
#include"Vector3.h"
#include "application/Collider.h"

class PlayerBullet :public Collider{
private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	std::unique_ptr<Object3d> obj3d_;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 速度
	Vector3 velocity_;

	// 寿命<frm>
	static const int32_t kLifeTime = 25 * 5;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	Vector3 center_;

	static inline const float kRadius_ = 1.0f;

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
	void Update(const Vector3& center);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProejection"></param>
	void Draw(const ViewProjection& viewProejection);

	/// <summary>
	/// ゲッター
	/// </summary>
	/// <returns></returns>
	bool IsDead() const { return isDead_; }
	float GetRadius() { return kRadius_; }
	Vector3 GetWorldPosition();

	void OnCollision();
};
