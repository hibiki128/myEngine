#pragma once
#include"application/Base/BaseObject.h"
class Player;
class Enemy :public BaseObject
{
	enum class Behavior {
		kRoot,
		kKnockback,
	};

public:
	/// ===================================================
	///public method
	/// ===================================================

	void Init(const std::string className)override;

	void Update()override;

	void Draw(const ViewProjection& viewProjection)override;

	void SetPlayer(Player* player) { player_ = player; }

	void SetPosition(const Vector3& position) { transform_.translation_ = position; }

	bool IsDead() { return isDead_; }

private:

	void Move();

	void BehaviorRootInitialize();

	void BehaviorKnockbackInitialize();

	void BehaviorRootUpdate();

	void BehaviorKnockbackUpdate();

	Vector3 GetCenterPosition()const override;
	Vector3 GetCenterRotation()const override;

	void OnCollision([[maybe_unused]] Collider* other)override;
	void OnCollisionEnter([[maybe_unused]] Collider* other)override;

private:
	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
	Behavior behavior_ = Behavior::kRoot;

	Player* player_ = nullptr;
	int HP = 10;
	bool isDead_ = false;

};

