#pragma once
#include "Audio.h"
#include"BaseScene.h"
#include "Input.h"
#include"Object3dCommon.h"
#include "SpriteCommon.h"
#include"ParticleCommon.h"
#include"ViewProjection.h"
#include"ParticleEmitter.h"
#include"Object3d.h"
#include"WorldTransform.h"
#include"DebugCamera.h"
#include"vector"
#include <Skydome.h>

class TitleScene :public BaseScene
{
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	ViewProjection* GetViewProjection()override { return &vP_; }

private:
	Audio* audio_;
	Input* input_;
	Object3dCommon* objCommon_;
	SpriteCommon* spCommon_;
	ParticleCommon* ptCommon_;

	ViewProjection vP_;
	WorldTransform wt_;
	WorldTransform wt2_;

	std::unique_ptr<ParticleEmitter> fall_;
	std::unique_ptr<ParticleEmitter> Break_;
	std::unique_ptr<ParticleEmitter> WeekBreak_;
	std::vector<std::unique_ptr<ParticleEmitter>> coreFractions_;
	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<Object3d> obj2_;

	std::unique_ptr<DebugCamera> debugCamera_;
};
