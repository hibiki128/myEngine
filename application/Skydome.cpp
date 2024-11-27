#include "Skydome.h"
#include "assert.h"

Skydome::Skydome() {}

Skydome::~Skydome() {}

void Skydome::Init() {
	obj3D_ = std::make_unique<Object3d>();
	obj3D_->Initialize("SkyDome/skydome.obj");
	transform_.Initialize();
	transform_.scale_ = { 200, 200, 200 };
}

void Skydome::Update() {
	transform_.UpdateMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection) {
	obj3D_->Draw(transform_, viewProjection);
}
