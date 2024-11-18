#include "GameScene.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "ParticleManager.h"
#include <ModelManager.h>
#include"SceneManager.h"
#include"ModelManager.h"

void GameScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();

	vp_.Initialize();

	//----------レール-----------
	rail_ = std::make_unique<rail>();
	rail_->Initialize(&vp_);
	//--------------------------

	cameraT_.Initialize();

	//----------レールカメラ-----------
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize(cameraT_);
	//-------------------------------

	//----------プレイヤー------------
	player_ = std::make_unique<Player>();
	Vector3 playerPos_ = { 0.0f,-0.37f,0.2f };
	player_->Initilaize(&vp_, playerPos_);
	player_->SetParent(&railCamera_->GetWorldTransform());
	player_->SetRailCamera(railCamera_.get());
	//-------------------------------

	//----------天球------------
	skydome_ = std::make_unique<skyDome>();
	skydome_->Initialize();
	//-------------------------

	//---------敵----------
	for (int i = 0; i < 33; ++i) {
		// 新しいenemyを作成
		auto newEnemy = std::make_unique<Enemy>();

		// 初期化処理
		newEnemy->Init();

		// 敵のリストに追加
		enemies_.push_back(std::move(newEnemy));

		// リスト内の敵数を基にAddItemを呼び出し
		enemies_.back()->AddItem(i);
	}
	//--------------------

	//-------スコア--------
	scoreManager_ = std::make_unique<ScoreManager>();
	scoreManager_->Initialize();
	scoreManager_->Setpos(Vector2(1200.0f, 645.0f));

	currentScore_ = 0;

	score_ = std::make_unique<Sprite>();
	score_->Initialize("score.png",Vector2(1120.0f,570.0f),{1,1,1,1},{0.5f,0.5f});
	score_->SetSize(score_->GetSize() * 0.5f);
	//--------------------
}

void GameScene::Finalize()
{

}

void GameScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("gameScene");
	if (ImGui::BeginTabBar("camera")) {
		if (ImGui::BeginTabItem("camera")) {
			ImGui::DragFloat3("translate", &vp_.translation_.x, 0.1f);
			ImGui::DragFloat3("rotate", &vp_.rotation_.x, 0.1f);
			ImGui::EndTabItem();
		}
		railCamera_->imgui();
		ImGui::EndTabBar();
	}
	AddEnemyByButton();
	player_->imgui();
	rail_->imgui();
	ImGui::End();
#endif // _DEBUG

	// 前回の生存状態を記録し、スコア加算を一度だけにするロジック
	for (auto& enemy : enemies_) {
		static std::unordered_map<Enemy*, bool> prevStates; // 各敵の前回状態を記録

		// 現在の生存状態を取得
		bool isCurrentlyAlive = enemy->IsAlive();

		// 前回状態が記録されていない場合は初期化
		if (prevStates.find(enemy.get()) == prevStates.end()) {
			prevStates[enemy.get()] = true; // 初期状態は「生存中」と仮定
		}

		// 前回は生存中で、現在は死亡している場合にスコアを加算
		if (prevStates[enemy.get()] && !isCurrentlyAlive) {
			currentScore_ += 50;
		}

		// 前回の状態を更新
		prevStates[enemy.get()] = isCurrentlyAlive;
	}

	//-----シーン切り替え-----
	if (railCamera_->IsFinish()) {
		sceneManager_->ChangeScene("TITLE");
	}
	//----------------------

	rail_->Update();

	vp_.UpdateMatrix();
	railCamera_->SetControlPoints(rail_->GetControlPoints());
	railCamera_->Update();
	vp_.matView_ = railCamera_->GetViewProjection().matView_;
	vp_.TransferMatrix();
	player_->SetRailCameraRotation(railCamera_->GetWorldTransform().rotation_);
	player_->Update();
	for (auto& enemy : enemies_) {
		enemy->Update();
	}
	skydome_->Update();
}

void GameScene::Draw()
{
	/// -------描画処理開始-------

		/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----
	player_->DrawUI();
	//------------------------

	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----
	skydome_->Draw(vp_);
	//rail_->IcoDraw();
	player_->Draw();
	for (auto& enemy : enemies_) {
		enemy->Draw(&vp_);
	}
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	rail_->railDraw();
	for (auto& enemy : enemies_) {
		enemy->DrawParticle(vp_);
	}
	//-----------------------------

	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----
	score_->Draw();
	scoreManager_->DrawScore(currentScore_, Vector2(45.0f,112.5f), Vector2(1200.0f, 570.0f));
	//------------------------

	/// ----------------------------------
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Draw();
#endif // _DEBUG
	/// -------描画処理終了-------
}

void GameScene::AddEnemyByButton()
{
	// TabBar "enemy" を開始
	if (ImGui::BeginTabBar("enemy")) {
		// "enemy" 内にある TabItem を追加
		if (ImGui::BeginTabItem("Enemies")) {

			// 次に各 enemy のタブを生成
			if (ImGui::BeginTabBar("EnemyTabBar")) {
				int index = 0;
				for (auto& enemy : enemies_) {
					// "enemy0", "enemy1" のようなタブ名を動的に生成
					std::string tabName = "enemy" + std::to_string(index);

					// タブアイテムの開始
					if (ImGui::BeginTabItem(tabName.c_str())) {
						// タブ内で表示する内容を記述 (ここに必要な情報を表示)
						enemy->imgui();

						// タブアイテムの終了
						ImGui::EndTabItem();
					}
					++index;
				}
				ImGui::EndTabBar();
			}

			// "Enemies" タブアイテムの終了
			ImGui::EndTabItem();
		}
		// "enemy" タブバーの終了
		ImGui::EndTabBar();
	}
}
