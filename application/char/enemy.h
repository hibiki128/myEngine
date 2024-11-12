#pragma once
#include"WorldTransform.h"
#include"Object3d.h"
#include"Collider.h"
#include"ViewProjection.h"
#include"GlobalVariables.h"
#include"ParticleEmitter.h"
class enemy :public Collider
{
public:

	void Init();

	void Update();

	void Draw(ViewProjection* vp_);
	void DrawParticle(const ViewProjection& _vp);

	Vector3 GetCenterPosition() const override;
	Vector3 GetCenterRotation() const override;
	AABB GetAABB() const override;

	void OnCollision([[maybe_unused]] Collider* other) override;

	void imgui();

	void AddItem(int enemyNum);

	bool IsAlive() { return isAlive_; }

	void ApplyVariables();
private:
	std::unique_ptr<ParticleEmitter> emitter_ = nullptr;
	std::unique_ptr<Object3d>obj3d_;
	WorldTransform wt_;
	GlobalVariables* variables_;
	std::string groupName_;
	std::string itemName;
	bool isAlive_;
	int HP_;
};
