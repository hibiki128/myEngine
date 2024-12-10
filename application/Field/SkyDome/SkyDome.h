#pragma once
#include"application/Base/BaseObject.h"

class SkyDome : public BaseObject
{
public:

	/// ===================================================
	///public method
	/// ===================================================

	void Init()override;

	void Update()override;

	void Draw(const ViewProjection& viewProjection)override;

	void DebugTransform(const std::string className)override;

private:
};

