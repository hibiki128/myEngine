#pragma once
#include"application/Base/BaseObject.h"
#include"Input.h"

class Player : public BaseObject,public Collider
{
public:

	/// ===================================================
	///public method
	/// ===================================================

	void Init()override;

	void Update()override;

	void Draw(const ViewProjection& viewProjection)override;

	void DebugTransform(const std::string className)override;

	Vector3 GetCenterPosition()const override;
	Vector3 GetCenterRotation()const override;

private:

	/// ===================================================
    ///private method
    /// ===================================================

	void Move();

};

