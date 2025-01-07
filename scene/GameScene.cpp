#include "GameScene.h"
#include <LightGroup.h>
#include"SceneManager.h"
#include <line/DrawLine3D.h>
#include <random>
#include"myEngine/Frame/Frame.h"


void GameScene::Finalize()
{

}

void GameScene::Initialize()
{

	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);

	/// ===================================================
	/// 生成
	/// ===================================================
	player_ = std::make_unique<Player>();
	//enemy_ = std::make_unique<Enemy>();
	followCamera_ = std::make_unique<FollowCamera>();
	skyDome_ = std::make_unique<SkyDome>();
	ground_ = std::make_unique<Ground>();

	/// ===================================================
	/// 初期化
	/// ===================================================
	player_->Init();
	//enemy_->Init();
	followCamera_->Init();
	skyDome_->Init();
	ground_->Init();

	/// ===================================================
	/// セット
	/// ===================================================

	followCamera_->SetTarget(&player_->GetWorldTransform());
	player_->SetCamera(followCamera_.get());
	//enemy_->SetPlayer(player_.get());
}

void GameScene::Update()
{
	// デバッグモード時の処理
#ifdef _DEBUG
	Debug();
#endif // _DEBUG

	// カメラ更新
	CameraUpdate();

	// シーン切り替え
	ChangeScene();

	// 敵を追加するためのタイマー更新
	spawnTimer_ += Frame::DeltaTime();

	// タイマーが spawnInterval_ を超えた場合に敵を追加
	if (spawnTimer_ >= spawnInterval_)
	{
		spawnTimer_ = 0.0f;  // タイマーリセット

		// 敵の数が5以下の場合に追加
		if (enemies_.size() < 5)
		{
			Vector3 playerPosition = player_->GetPosition();
			SpawnEnemies({ playerPosition.x,1.0f,playerPosition.z }); // プレイヤー周囲に敵を生成
		}
	}

	// 死亡した敵をリストから削除し、カウントを更新
	auto it = enemies_.begin();
	while (it != enemies_.end())
	{
		if ((*it)->IsDead())  // 死亡している場合
		{
			it = enemies_.erase(it);  // リストから削除
			++deadEnemiesCount_;  // 死亡した敵をカウント
		}
		else
		{
			++it;
		}
	}

	// 他のオブジェクトの更新処理
	player_->Update();
	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}
	skyDome_->Update();
	ground_->Update();
}


void GameScene::Draw()
{
	/// -------描画処理開始-------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----

	//------------------------

	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----
	skyDome_->Draw(vp_);
	ground_->Draw(vp_);
	player_->Draw(vp_);
	for (auto& enemy : enemies_) {
		enemy->Draw(vp_);
	}
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	player_->DrawParticle(vp_);
	//-----------------------------

	//-----線描画-----
	DrawLine3D::GetInstance()->Draw(vp_);
	//---------------

	/// ----------------------------------

	/// -------描画処理終了-------
}

void GameScene::DrawForOffScreen()
{
	/// -------描画処理開始-------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----

	//------------------------

	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----

	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------

	//-----------------------------


	/// ----------------------------------

	/// -------描画処理終了-------
}

void GameScene::Debug()
{
	ImGui::Begin("GameScene:Debug");

	// 最初のタブバー
	if (ImGui::BeginTabBar("1")) {
		debugCamera_->imgui();
		LightGroup::GetInstance()->imgui();
		ImGui::EndTabBar();
	}

	ImGui::End(); // ダイアログの終了
	ImGui::Begin("ObjData");
	player_->imgui();
	ImGui::End();
	// その他のデバッグ情報
	player_->DebugTransform("プレイヤー ");
	//enemy_->DebugTransform("エネミー ");
	followCamera_->imgui();
}

void GameScene::CameraUpdate()
{
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	}
	else {
		followCamera_->Update();
		vp_.matWorld_ = followCamera_->GetViewProjection().matWorld_;
		vp_.matView_ = followCamera_->GetViewProjection().matView_;
		vp_.matProjection_ = followCamera_->GetViewProjection().matProjection_;
		//vp_.UpdateMatrix();
	}
}

void GameScene::ChangeScene()
{
	if (deadEnemiesCount_ > 5) {
		sceneManager_->NextSceneReservation("TITLE");
	}
}

void GameScene::AddEnemy(std::unique_ptr<Enemy> enemy)
{
	enemies_.push_back(std::move(enemy));
}

void GameScene::SpawnEnemy(const Vector3& position) {
	// 敵の生成
	std::unique_ptr<Enemy> enemy;

	enemy = std::make_unique<Enemy>();
	// 敵キャラモデル
	enemy->Init();
	enemy->SetPosition(position);
	enemy->SetPlayer(player_.get());
	// 敵を追加
	AddEnemy(std::move(enemy));
}

void GameScene::SpawnEnemies(const Vector3& position)
{
	const int maxEnemies = 5;
	const float radius = 30.0f;

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<> angleDist(0.0, 360.0);

	std::uniform_real_distribution<> distDist(0.0, radius);


	float angle = float(angleDist(gen));

	float dist = float(distDist(gen));

	// ランダムな位置を計算
	Vector3 enemyPosition;
	enemyPosition.x = position.x + dist * cosf(degreesToRadians(angle));
	enemyPosition.z = position.z + dist * sinf(degreesToRadians(angle));
	enemyPosition.y = position.y;

	// 敵を生成
	SpawnEnemy(enemyPosition);

}
