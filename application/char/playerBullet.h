#pragma once
#include"object3d.h"
#include"WorldTransform.h"
#include"math/Vector3.h"
#include"Collider.h"
#include"Quaternion.h"
class playerBullet :public Collider{
private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	std::unique_ptr<Object3d> obj_ = nullptr;

	// 寿命<frm>
	static const int32_t kLifeTime = 10 * 10;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isFire_ = false;

	static inline const float kRadius_ = 1.0f;

	void OnCollisionEnter([[maybe_unused]] Collider* other) override;

	Vector3 GetCenterPosition()const override;
	Vector3 GetCenterRotation()const override;

	AABB GetAABB()const override;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	/// <param name="position"></param>
	void Initialize();

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
	/// ゲッター
	/// </summary>
	/// <returns></returns>
	bool IsFire() const { return isFire_; }
	float GetRadius() { return kRadius_; }
	Vector3 GetWorldPosition();

	/// <summary>
	/// setter
	/// </summary>
	/// <param name="isFire"></param>
	void SetFire(bool isFire) { isFire_ = isFire; }
	void SetPosition(const Vector3& position) {
		worldTransform_.translation_ = position;
	}
	void SetParent(const WorldTransform* parent);
	void SetRotation(const Quaternion& rotation) {
		// クォータニオンをオイラー角に変換して rotation_ に代入
		worldTransform_.rotation_ = rotation.ToEulerAngles();
		// 行列を更新して、新しい回転を反映させる
		worldTransform_.UpdateMatrix();
	}
};
