#include "skyDome.h"

void skyDome::Initialize()
{
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("field/skydome.obj");
	// ワールド変換の初期化
	worldTransform_.Initialize();
}

void skyDome::Update()
{
	// 行列定数バッファに転送
	worldTransform_.UpdateMatrix();
}

void skyDome::Draw(const ViewProjection& vP_)
{
	// 3Dモデルを描画
	obj_->Draw(worldTransform_, vP_,nullptr,false);
}
