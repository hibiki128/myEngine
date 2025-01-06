#pragma once
#include"application/Base/BaseObject.h"
class Weapon : public BaseObject
{
public:
	/// ===================================================
	///public method
	/// ===================================================

	void Init()override;

	void Update()override;

	void Draw(const ViewProjection& viewProjection)override;

	void DebugTransform(const std::string className)override;

	void SetParent(const WorldTransform& worldTransform) { transform_.parent_ = &worldTransform; }

	void SetScale(const Vector3& scale) { transform_.scale_ = scale; }

	Vector3 GetCenterPosition()const override;
	Vector3 GetCenterRotation()const override;

private:
};

