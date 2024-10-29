#pragma once
#include "Audio.h"
#include"BaseScene.h"
#include "Input.h"
#include"Object3dCommon.h"
#include "SpriteCommon.h"
#include"ParticleCommon.h"
#include"line/LineManager.h"
#include"ViewProjection.h"
#include"memory"
#include"DebugCamera.h"
#include"Sprite.h"
#include"Object3d.h"
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


	void LineDraw();

private:
	Audio* audio_;
	Input* input_;
	Object3dCommon* objCommon_;
	SpriteCommon* spCommon_;
	ParticleCommon* ptCommon_;
	ViewProjection vP_;
	
	std::unique_ptr<LineManager> line_;

	// 制御点リスト
	std::vector<Vector3> controlPoints = { Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f) };
};

