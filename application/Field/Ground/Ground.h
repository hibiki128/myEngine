#pragma once
#include"application/Base/BaseObject.h"

class Ground : public BaseObject
{
public:

	/// ===================================================
	///public method
	/// ===================================================

	void Init(const std::string className)override;

	void Update()override;

	void Draw(const ViewProjection& viewProjection)override;

};

