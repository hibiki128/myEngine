#pragma once
#include"application/Base/BaseObject.h"
#include"Input.h"
#include"Vector3.h"
#include"Weapon/Weapon.h"

class FollowCamera;
class Player : public BaseObject, public Collider
{
public:

	/// ===================================================
	///public method
	/// ===================================================

	void Init()override;

	void Update()override;

	void Draw(const ViewProjection& viewProjection)override;

	void DebugTransform(const std::string className)override;

	void imgui();

	void SetCamera(FollowCamera* camera) { camera_ = camera; }

	Vector3 GetCenterPosition()const override;
	Vector3 GetCenterRotation()const override;

	void OnCollision([[maybe_unused]] Collider* other)override;

private:

	/// ===================================================
	///private method
	/// ===================================================

	void Move();

	void Rotation();

private:
	FollowCamera* camera_;
	std::unique_ptr<Weapon>weapon_;

	Vector3 move = { 0.0f,0.0f,0.0f };
	Vector3 acce = { 0.1f,0.0f,0.1f };

	float dashCoolTime_;
	float dashSpeed_;
	float kMoveSpeed = 0.3f;
	float kDashSpeed = 1.75f;
	float kDashDecay = 0.97f;

};

