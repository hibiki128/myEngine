#pragma once
#include "Audio.h"
#include "BaseScene.h"
#include "Input.h"
#include "Object3dCommon.h"
#include "SpriteCommon.h"
#include "ParticleCommon.h"
#include "ViewProjection.h"
#include "application/Screw.h"
#include "application/player/Player.h"
#include "application/player/PlayerBullet.h"
#include "application/Enemy.h"

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


	//void Attack();

	//void Bulletdelete();


	//Vector3 GetWorldPosition() {
	//	// ワールド座標を入れる変数
	//	Vector3 worldPos;
	//	// ワールド行列の平行移動成分を取得(ワールド座標)
	//	worldPos.x = worldTransform_.matWorld_.m[3][0];
	//	worldPos.y = worldTransform_.matWorld_.m[3][1];
	//	worldPos.z = worldTransform_.matWorld_.m[3][2];

	//	return worldPos;
	//}

private:
	Audio* audio_;
	Input* input_;
	Object3dCommon* objCommon_;
	SpriteCommon* spCommon_;
	ParticleCommon* ptCommon_;

	ViewProjection viewProjection_ = {};
	std::unique_ptr <Screw> screw_;
	std::unique_ptr <Player> player_;
	std::unique_ptr <Enemy> enemy_;
	std::unique_ptr <Object3d> particleModel = nullptr;
};
