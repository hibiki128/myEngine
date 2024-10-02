#pragma once
#include"memory"
#include"Sprite.h"
#include"vector"
#include"Object3dCommon.h"
#include "Input.h"
#include "SpriteCommon.h"
#include "Audio.h"
#include"Object3d.h"
#include"BaseScene.h"
#include"WorldTransform.h"

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

private:
	Audio* audio_;
	Input* input_;
	Object3dCommon* objCommon_;
	SpriteCommon* spCommon_;
	uint32_t handle;
	std::unique_ptr<Object3d> object3d[2];
	std::vector<std::unique_ptr<Sprite>> sprites;
	std::vector<Vector2> positions;
	std::string modelFilePath[2];
	WorldTransform modelWorldTransform[2];
	ViewProjection viewProjection;
};

