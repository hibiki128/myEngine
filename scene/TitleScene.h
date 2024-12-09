#pragma once
#include "Audio.h"
#include"BaseScene.h"
#include "Input.h"
#include"Object3dCommon.h"
#include "SpriteCommon.h"
#include"ParticleCommon.h"
#include"ViewProjection.h"
#include"Object3d.h"
#include"WorldTransform.h"
#include"Sprite.h"
#include"line/DrawLine3D.h"

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

	void DrawSkeleton(Object3d* obj);

	ViewProjection* GetViewProjection()override { return &vp_; }

	Vector3 ExtractTranslation(const Matrix4x4& matrix);
private:
	Audio* audio_;
	Input* input_;
	Object3dCommon* objCommon_;
	SpriteCommon* spCommon_;
	ParticleCommon* ptCommon_;

	ViewProjection vp_;

	std::unique_ptr<Object3d> obj_;
	WorldTransform wt_;

	DrawLine3D* liner_;
};
