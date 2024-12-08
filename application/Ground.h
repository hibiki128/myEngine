#pragma once
#include"Object3d.h"
#include"WorldTransform.h"
#include"ViewProjection.h"
#include"Collider.h"
class Ground : public Collider
{
public:
	void Initialize();

	void Update();

	void Draw(const ViewProjection& viewProjection);

	Vector3 GetCenterPosition() const override;
	Vector3 GetCenterRotation() const override;
private:

	std::unique_ptr<Object3d> obj_;
	WorldTransform wt_;
};

