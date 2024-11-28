#include "Skydome.h"
#include "assert.h"

Skydome::Skydome() {}

Skydome::~Skydome() {}

void Skydome::Init() {
	obj3D_ = std::make_unique<Object3d>();
	obj3D_->Initialize("SkyDome/skydome.obj");
	transform_.Initialize();
	transform_.scale_ = { 200, 200, 200 };

	for (int i = 0; i < 4; i++) {
		std::unique_ptr<ParticleEmitter> emitter_;
		emitter_ = std::make_unique<ParticleEmitter>();
		if (i == 0) {
			emitter_->Initialize("starrySky_left", "game/starry.obj");
		}
		else if (i == 1) {
			emitter_->Initialize("starrySky_right", "game/starry.obj");
		}
		else if (i == 2) {
			emitter_->Initialize("starrySky_front", "game/starry.obj");
		}
		else if (i == 3) {
			emitter_->Initialize("starrySky_back", "game/starry.obj");
		}
		starrySkys_.push_back(std::move(emitter_));
	}

}

void Skydome::Update() {
	transform_.UpdateMatrix();
	
}

void Skydome::Draw(ViewProjection& viewProjection) {
	obj3D_->Draw(transform_, viewProjection);
}

void Skydome::DrawParticle(const ViewProjection& viewProjection)
{
	for (auto& starrySky_ : starrySkys_) {
		starrySky_->Update(viewProjection);
		starrySky_->Draw();
	}
}
