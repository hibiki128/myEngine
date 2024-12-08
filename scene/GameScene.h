#pragma once
#include "Audio.h"
#include "BaseScene.h"
#include "Input.h"
#include "Object3dCommon.h"
#include "SpriteCommon.h"
#include "ParticleCommon.h"
#include"ViewProjection.h"
#include"DebugCamera.h"
#include"application/Ground.h"
#include"application/Meteor.h"
#include"vector"

class GameScene : public BaseScene
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

	ViewProjection* GetViewProjection()override { return &vp_; }
private:
	void Debug();

	void CameraUpdate();

	void ChangeScene();

	void GenerateMeteor();
private:

	Audio* audio_;
	Input* input_;
	Object3dCommon* objCommon_;
	SpriteCommon* spCommon_;
	ParticleCommon* ptCommon_;

	// ビュープロジェクション
	ViewProjection vp_;
	std::unique_ptr<DebugCamera> debugCamera_;

	std::unique_ptr<Ground>ground_;

	std::vector<std::unique_ptr<Meteor>>meteors_;
};
