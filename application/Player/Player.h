#pragma once
#include"application/Base/BaseObject.h"
#include"Input.h"
#include"Vector3.h"
#include"Weapon/Weapon.h"
#include"ParticleEmitter.h"
#include"Easing.h"
#include"GlobalVariables.h"

class FollowCamera;
class Player : public BaseObject
{
private:
	enum class Behavior {
		kRoot,
		kAttack,
	};

public:

	/// ===================================================
	///public method
	/// ===================================================

	void Init()override;

	void Update()override;

	void Draw(const ViewProjection& viewProjection)override;

	void DrawParticle(const ViewProjection& viewProjection);

	void DebugTransform(const std::string className)override;

	void imgui();

	void SetCamera(FollowCamera* camera) { camera_ = camera; }

	Vector3 GetPosition() { return transform_.translation_; }
	Vector3 GetRotation() { return transform_.rotation_; }
	Matrix4x4 GetWorldMatrix() { return transform_.matWorld_; }

	Vector3 GetCenterPosition()const override;
	Vector3 GetCenterRotation()const override;

	void OnCollision([[maybe_unused]] Collider* other)override;
	int GetComboStage() { return comboStage_; }

private:

	/// ===================================================
	///private method
	/// ===================================================

	void Move();

	void Rotation();

	void AffterEffect();

	void ArmFollow();

	void Attack();

	void NormalAttack();

	void UpAttack();

	void DownAttack();

	void RowlingAttack();

	void BehaviorRootInitialize();

	void BehaviorAttackInitialize();

	void BehaviorRootUpdate();

	void BehaviorAttackUpdate();

	void HandleCombo();

	void ProcessComboInput();

private:
	FollowCamera* camera_;
	std::unique_ptr<Weapon>weapon_;
	std::unique_ptr<ParticleEmitter> afterImageEmitter_;
	std::unique_ptr<Object3d> shadow_;

	// プレイヤーの部位
	WorldTransform R_arm_wt;
	WorldTransform L_arm_wt;
	WorldTransform Shadow_;
	std::unique_ptr<Object3d>R_armModel_;
	std::unique_ptr<Object3d>L_armModel_;

	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
	Behavior behavior_ = Behavior::kRoot;

	Vector3 move = { 0.0f,0.0f,0.0f };
	Vector3 acce = { 0.1f,0.0f,0.1f };
	Vector3 speed = { 0.0f,0.0f,0.0f };
	Vector3 R_arm_offset = { 0.0f,0.0f,0.0f };
	Vector3 L_arm_offset = { 0.0f,0.0f,0.0f };

	float dashCoolTime_;
	float dashSpeed_;
	float kMoveSpeed = 0.3f;
	float kDashSpeed = 1.75f;
	float kDashDecay = 0.97f;

	int comboStage_ = 0;  // 現在のコンボステージ
	float comboTimer_ = 0.0f;  // コンボ入力待機タイマー
	float attackTimer_ = 0.0f;
	bool isNextAttack_ = false;
	bool isJKeyPressed_ = false;
	Vector3 startAngle;
	Vector3 endAngle;
	Vector3 startPosition;
	Vector3 endPosition;
	float easeTMax;
	float fallSpeed = 0.0f;

	GlobalVariables* globalVariables = nullptr;
	const char* groupName = nullptr;
};

