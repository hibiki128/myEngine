#pragma once
#include "Object3d.h"
#include "application/char/playerBullet.h"
#include "WorldTransform.h"
#include "Input.h"
#include"list"
#include <ViewProjection.h>
#include"Sprite.h"
#include <Quaternion.h>

class Player {
private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform Reticle3Dwt_;

	// ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;

	// モデル
	std::unique_ptr<Object3d> obj_ = nullptr;
	std::unique_ptr<Object3d> reticleObj_ = nullptr;

	// キーボード入力
	Input* input_ = nullptr;

	// 弾
	std::unique_ptr<playerBullet> LeftBullet_;
	std::unique_ptr<playerBullet> RightBullet_;
	std::unique_ptr<playerBullet> CenterBullet_;

	std::unique_ptr<Sprite> reticle_ = nullptr;
	Vector2 spritePos;
	Vector3 LeftBulletOffset;
	Vector3 RightBulletOffset;
	Vector3 CenterBulletOffset;

	static inline const float kRadius_ = 1.0f;
public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initilaize(ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void DrawUI();

	void imgui();

	void AimBulletAtReticle();

	Quaternion CalculateRotationFromDirection(const Vector3& direction) {
		Quaternion rotation;
		rotation.SetFromTo({ 0.0f, 0.0f, 1.0f }, direction); // Z軸からdirectionに向ける回転
		return rotation;
	}

	/// <summary>
	/// ゲッター
	/// </summary>
	Vector3 GetWorldPosition();
	Vector3 GetLocalPosition() { return worldTransform_.translation_; }
	Vector3 GetWorldReticlePosition();

	/// <summary>
	/// セッター
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(const WorldTransform* parent);

	float GetRadius() { return kRadius_; };
private:
	/// <summary>
	/// 回転
	/// </summary>
	void Rotate();

	/// <summary>
	/// 移動
	/// </summary>
	void Move();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// 衝突判定
	/// </summary>
	void OnCollision();

	/// <summary>
	/// レティクルの設定
	/// </summary>
	void SetReticle();

	/// <summary>
	/// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	/// </summary>
	/// <returns></returns>
	void TramsformReticle();

	/// <summary>
	/// 照準操作
	/// </summary>
	void MoveAim();

};
