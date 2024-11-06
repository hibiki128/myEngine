#pragma once
#include"Vector3.h"
#include"WorldTransform.h"
#include"Object3d.h"
#include"ViewProjection.h"
#include"GlobalVariables.h"
struct AABB {
	Vector3 min; //!< 最小点
	Vector3 max; //!< 最大点
};
class Collider {
public:

	Collider();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Collider();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ワールドトランスフォームの更新
	/// </summary>
	void UpdateWorldTransform();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="model"></param>
	/// <param name="viewProjection"></param>
	void DrawSphere(const ViewProjection& viewProjection);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="model"></param>
	/// <param name="viewProjection"></param>
	void DrawAABB(const ViewProjection& viewProjection);

	/// <summary>
	/// 当たってる間
	/// </summary>
	/// <param name="other"></param>
	virtual void OnCollision([[maybe_unused]] Collider* other) {};

	/// <summary>
	/// 当たった瞬間
	/// </summary>
	/// <param name="other"></param>
	virtual void OnCollisionEnter([[maybe_unused]] Collider* other) {};

	/// <summary>
	/// 当たり終わった瞬間
	/// </summary>
	/// <param name="other"></param>
	virtual void OnCollisionOut([[maybe_unused]] Collider* other) {};

	/// <summary>
	/// 当たったかどうか色で分かるようにしたいなら呼んでください
	/// </summary>
	void SetHitColor() {
		color_.SetColor({ 1.0f,0.0f,0.0f,1.0f });
	}

	/// <summary>
	/// 元の色
	/// </summary>
	void SetDefaultColor() {
		color_.SetColor({ 0.0f,0.0f,0.0f,1.0f });
	}

	/// <summary>
	/// getter
	/// </summary>
	/// <returns></returns>
	// 半径を取得
	float GetRadius() { return radius_; }
	// 中心座標を取得
	virtual Vector3 GetCenterPosition() const = 0;

	virtual AABB GetAABB()const = 0;
	// 種別IDを取得
	uint32_t GetTypeID() const { return typeID_; }
	// 種別IDを設定
	void SetTypeID(uint32_t typeID) { typeID_ = typeID; }

	Vector3 GetCenterPos() { return Cubewt_.translation_; }
	AABB GetAABBPos() { return aabb; }

	/// <summary>
	/// setter
	/// </summary>
	/// <param name="radius"></param>
	void SetRadius(float radius) { radius_ = radius; }

	void SetIsColliding(bool colliding) { wasColliding = isColliding; isColliding = colliding; }
	bool IsColliding() const { return isColliding; }
	bool WasColliding() const { return wasColliding; }

private:
	void ApplyVariables();

private:

	// 衝突半径
	float radius_ = 1.0f;
	// ワールドトランスフォーム
	WorldTransform Cubewt_;
	WorldTransform AABBwt_;
	// 種別ID
	uint32_t typeID_ = 0u;
	std::unique_ptr<Object3d>sphere_;
	std::unique_ptr<Object3d>AABB_;

	GlobalVariables* variables_;
	std::string groupName;
	AABB aabb;
	Vector3 aabbCenter;
	Vector3 aabbScale;

	static int counter; // 静的カウンタ
	Vector3 SphereOffset = { 0.0f,0.0f,0.0f };
	Vector3 MinOffset = { 0.0f,0.0f,0.0f };
	Vector3 MaxOffset = { 0.0f,0.0f,0.0f };

	ObjColor color_;

	bool isColliding = false;   // 現在のフレームの衝突状態
	bool wasColliding = false;  // 前フレームの衝突状態
};
