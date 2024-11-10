#include "CollisionManager.h"
#include "myMath.h"
#include "GlobalVariables.h"
#include"Object3dCommon.h"
std::list<Collider*>  CollisionManager::colliders_;
void CollisionManager::Reset() {
	// リストを空っぽにする
	colliders_.clear();
}

// Colliderを削除する
void CollisionManager::RemoveCollider(Collider* collider) {
	// colliderが存在するか確認し、存在すれば削除
	auto it = std::find(colliders_.begin(), colliders_.end(), collider);
	if (it != colliders_.end()) {
		colliders_.erase(it);
	}
}

void CollisionManager::Initialize() {
	const char* groupName = "Collider";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	// グローバル変数を追加
	globalVariables->AddItem(groupName, "visible", visible);
	globalVariables->AddItem(groupName, "sphereCollision", sphereCollision);
	globalVariables->AddItem(groupName, "aabbCollision", aabbCollision);
}


void CollisionManager::UpdateWorldTransform() {
	ApplyGlobalVariables();
	// 非表示なら抜ける
	if (!visible) {
		return;
	}
	// 全てのコライダーについて
	for (Collider* collider : colliders_) {
		// 更新
		collider->UpdateWorldTransform();
	}
}

void CollisionManager::Draw(const ViewProjection& viewProjection) {
	// 非表示なら抜ける
	if (!visible) {
		return;
	}
	// 全てのコライダーについて
	for (Collider* collider : colliders_) {
		if (!collider->IsCollisionEnabled()) {
			continue;
		}
		if (sphereCollision) {
			// 描画
			collider->DrawSphere(viewProjection);
		}
		if (aabbCollision) {
			collider->DrawAABB(viewProjection);
		}
	}
}

void CollisionManager::Update()
{
	CheckAllCollisions();
	UpdateWorldTransform();
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	// コリジョンが無効化されている場合はチェックをスキップ
	if (!colliderA->IsCollisionEnabled() || !colliderB->IsCollisionEnabled()) {
		return;
	}

	bool isCollidingNow = false;

	// 球の衝突チェック
	if (sphereCollision) {
		Vector3 posA = colliderA->GetCenterPos();
		Vector3 posB = colliderB->GetCenterPos();
		float distance = (posA - posB).Length();
		isCollidingNow = (distance <= colliderA->GetRadius() + colliderB->GetRadius());
	}

	// AABBの衝突チェック
	if (aabbCollision && !isCollidingNow) {
		isCollidingNow = IsCollision(colliderA->GetAABBPos(), colliderB->GetAABBPos());
	}

	// 衝突状態の変化に応じたコールバックの呼び出し
	if (isCollidingNow) {
		if (!colliderA->WasColliding() && !colliderB->WasColliding()) {
			colliderA->OnCollisionEnter(colliderB);
			colliderB->OnCollisionEnter(colliderA);
		}
		else {
			colliderA->OnCollision(colliderB);
			colliderB->OnCollision(colliderA);
		}
	}
	else {
		if (colliderA->WasColliding() || colliderB->WasColliding()) {
			colliderA->OnCollisionOut(colliderB);
			colliderB->OnCollisionOut(colliderA);
		}
	}

	colliderA->SetIsColliding(isCollidingNow);
	colliderB->SetIsColliding(isCollidingNow);
}

void CollisionManager::CheckAllCollisions() {
	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;

		// イテレータBはイテレータAの次の要素から回す（重複判定を回避）
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;

			// ベアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}

}

void CollisionManager::AddCollider(Collider* collider)
{
	// コライダーをリストに追加する
	colliders_.push_back(collider);
}

void CollisionManager::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Collider";
	visible = globalVariables->GetBoolValue(groupName, "visible");
	sphereCollision = globalVariables->GetBoolValue(groupName, "sphereCollision");
	aabbCollision = globalVariables->GetBoolValue(groupName, "aabbCollision");
}

bool CollisionManager::IsCollision(const AABB& aabb1, const AABB& aabb2) {
	// 軸ごとに判定
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true;
	}
	return false;
}

