#pragma once
#include"application/Base/BaseObject.h"

class Player : public BaseObject
{
public:

	/// ===================================================
	///public method
	/// ===================================================
	
	void Init()override;

	void Update()override;

	void Draw(const ViewProjection& viewProjection)override;

	void DebugTransform(const std::string className)override;

	/*Vector3 GetCenterPosition()const override;
	Vector3 GetCenterRotation()const override;*/

private:
};

