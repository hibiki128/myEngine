#pragma once
#include"Object3d.h"
#include"WorldTransform.h"
#include"ViewProjection.h"
#include"Collider.h"
#include <random>
#include"ParticleEmitter.h"

using namespace std;

class Meteor : public Collider
{
public:
	void Initialize();

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void DrawParticle(const ViewProjection& viewProjection);

	Vector3 GetCenterPosition() const override;
	Vector3 GetCenterRotation() const override;
	void OnCollisionEnter([[maybe_unused]] Collider* other)override;

	bool IsAlive() { return isDeath_; }

private:

private:

	unique_ptr<Object3d> obj_;
	WorldTransform wt_;
	bool isAlive_;
	bool isDeath_;

	float counter_ = 0.0f;
	float deathCounter_ = 0.0f;

	// ランダム生成
	std::random_device rd_;
	std::mt19937 mt_{ rd_() };
	std::uniform_real_distribution<float> randX_{ -100.0f, 100.0f };
	std::uniform_real_distribution<float> randZ_{ -100.0f, 100.0f };

	std::unique_ptr<ParticleEmitter>fallEmitter_;
	std::vector<std::unique_ptr<ParticleEmitter>> coreFractions_;
};

