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


	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
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
	rail_->IcoDraw();
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
			// "Enemies" 内で Add Enemy ボタンを表示し、新しいエネミーを追加
			if (ImGui::Button("Add Enemy")) {
				// 新しいenemyを作成
				auto newEnemy = std::make_unique<enemy>();

				// リスト内の敵数を基に初期化
				int enemyNum = static_cast<int>(enemies_.size());
				newEnemy->Init();  // Init関数を呼び出して初期化

				// 初期化後、リストに追加
				enemies_.push_back(std::move(newEnemy));

				// AddItem関数を呼び出してアイテムを追加
				enemies_.back()->AddItem(enemyNum);
			}

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
