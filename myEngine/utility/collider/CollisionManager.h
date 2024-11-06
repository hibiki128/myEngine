#pragma once
#include"SceneManager.h"
#include"list"
#include"Collider.h"
#include"Object3d.h"
class CollisionManager {
private:
	// コライダー
	static std::list<Collider*> colliders_;

	bool visible = true;

	bool sphereCollision = true;

	bool aabbCollision = true;

public:
	/// <summary>
	/// リセット
	/// </summary>
	static void Reset();

	static void RemoveCollider(Collider* collider);

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
	/// <param name="viewProjection"></param>
	void Draw(const ViewProjection& viewProjection);


	void Update();

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="colliderA"></param>
	/// <param name="colliderB"></param>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	/// <summary>
	/// 全ての当たり判定チェック
	/// </summary>
	void CheckAllCollisions();

	/// <summary>
	/// コライダーの登録
	/// </summary>
	static void AddCollider(Collider* collider);

	// 調整項目の適用
	void ApplyGlobalVariables();

	bool IsCollision(const AABB& aabb1, const AABB& aabb2);
};
