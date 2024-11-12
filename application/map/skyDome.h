#pragma once
#include "Object3d.h"
#include "WorldTransform.h"

class skyDome {
public:
	void Initialize();

	void Update();

	void Draw(const ViewProjection& vP_);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> obj_ = nullptr;
};
