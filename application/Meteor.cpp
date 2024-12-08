#include "Meteor.h"
#include"Input.h"
#include"Ground.h"
#include"myEngine/Frame/Frame.h"

void Meteor::Initialize()
{
	obj_ = make_unique<Object3d>();
	obj_->Initialize("debug/sphere.obj");
	wt_.Initialize();
	wt_.translation_.x = randX_(mt_);  // X軸ランダム
	wt_.translation_.y = 100.0f;        // 固定位置
	wt_.translation_.z = randZ_(mt_);  // Z軸ランダム
	wt_.scale_ = { 7.5f,7.5f,7.5f };
	Collider::SetAABBScale({ 4.5f, 4.5f, 4.5f });
	isAlive_ = true;
	counter_ = 0.0f;

	fallEmitter_ = std::make_unique<ParticleEmitter>();
	fallEmitter_->Initialize("fall", "Particle/Cube.obj");
	for (int i = 0; i < 3; i++) {
		std::unique_ptr<ParticleEmitter> emitter_;
		emitter_ = std::make_unique<ParticleEmitter>();
		if (i == 0) {
			emitter_->Initialize("coreBreak1", "Particle/coreFraction1.obj");
		}
		else if (i == 1) {
			emitter_->Initialize("coreBreak2", "Particle/coreFraction2.obj");
		}
		else if (i == 2) {
			emitter_->Initialize("coreBreak3", "Particle/coreFraction3.obj");
		}

		coreFractions_.push_back(std::move(emitter_));
	}

}

void Meteor::Update()
{
	counter_ += Frame::DeltaTime();

	wt_.translation_.y -= 0.5f;
	wt_.rotation_ += { 0.2f, 0.2f, 0.2f };
	wt_.UpdateMatrix();

	fallEmitter_->SetPosition(wt_.translation_);

	if (!isAlive_) {
		Collider::SetCollisionEnabled(false);
		fallEmitter_->SetCount(0);
	}
	else {
		fallEmitter_->SetCount(5);
	}
}

void Meteor::Draw(const ViewProjection& viewProjection)
{
	if (isAlive_) {
		obj_->Draw(wt_, viewProjection);
	}
}

void Meteor::DrawParticle(const ViewProjection& viewProjection)
{
	fallEmitter_->Update(viewProjection);
	fallEmitter_->Draw();
	for (auto& coreFraction_ : coreFractions_) {
		coreFraction_->UpdateOnce(viewProjection);
		coreFraction_->Draw();
	}
}

Vector3 Meteor::GetCenterPosition() const
{
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };//ローカル座標のオフセット
	// ワールド座標に変換
	Vector3 worldPos = Transformation(offset, wt_.matWorld_);

	return worldPos;
}

Vector3 Meteor::GetCenterRotation() const
{
	return Vector3();
}

void Meteor::OnCollisionEnter(Collider* other)
{
	if (dynamic_cast<Ground*>(other) && counter_ > 1.0f) {
		isAlive_ = false;
		for (auto& coreFraction_ : coreFractions_) {
			coreFraction_->SetPosition(wt_.translation_);
			coreFraction_->SetActive(false);
		}
	}
}

