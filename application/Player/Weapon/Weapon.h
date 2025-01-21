#pragma once
#include"application/Base/BaseObject.h"
#include"ParticleEmitter.h"
class Weapon : public BaseObject
{
public:
	/// ===================================================
	///public method
	/// ===================================================

	void Init(const std::string className)override;

	void Update()override;

	void Draw(const ViewProjection& viewProjection)override;

	void DrawParticle();

	void Debug();

	void ParticleUpdate(const ViewProjection& viewProjection);

	void SetParent(const WorldTransform& worldTransform) { transform_.parent_ = &worldTransform; }

	void SetScale(const Vector3& scale) { transform_.scale_ = scale; }

	void SetPosition(const Vector3& position) { transform_.translation_ = position; }

	void SetRotation(const Vector3& rotation) { transform_.rotation_ = rotation; }
	
	void SetCount(const int count) { afterImageEmitter_->SetCount(count); }

	Vector3 GetCenterPosition()const override;
	Vector3 GetCenterRotation()const override;

private:
	std::unique_ptr<ParticleEmitter> afterImageEmitter_;
};

