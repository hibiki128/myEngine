#pragma once
#include "Audio.h"
#include"BaseScene.h"
#include "Input.h"
#include"Object3dCommon.h"
#include "SpriteCommon.h"
#include"ParticleCommon.h"
#include"ViewProjection.h"
#include"memory"
#include"application/map/rail.h"
#include"application/camera/RailCamera.h"
#include"application/char/player.h"
#include"application/char/enemy.h"
#include"application/map/skyDome.h"

#include"application/ScoreManager.h"

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

	// ImGuiのボタンで敵を追加
	void AddEnemyByButton();

private:
	Audio* audio_;
	Input* input_;
	Object3dCommon* objCommon_;
	SpriteCommon* spCommon_;
	ParticleCommon* ptCommon_;
	ViewProjection vp_;

	std::unique_ptr<rail> rail_;
	std::unique_ptr<RailCamera> railCamera_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<skyDome> skydome_;
	std::unique_ptr<ScoreManager>scoreManager_;

    std::list<std::unique_ptr<Enemy>> enemies_;
	int currentScore_ = 0;

	WorldTransform cameraT_;
};
