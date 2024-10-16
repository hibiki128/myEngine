#include "GameScene.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "ParticleManager.h"
#include <ModelManager.h>
#include"SceneManager.h"
#include"ModelManager.h"
#include "application/CollisionManager.h"

void GameScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();

	viewProjection_.Initialize();

	screw_ = std::make_unique<Screw>();
	screw_->Initialize();

	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetScrewPtr(screw_.get());

	enemy_= std::make_unique<Enemy>();
	enemy_->Initialize();

}

void GameScene::Finalize()
{

}

void GameScene::Update()
{
	CollisionManager* collisoinManager = CollisionManager::GetInstance();
	collisoinManager->ListReset();

	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("TITLE");
	}
	//----------------------


	/*screw_->Update();*/
	player_->Update();
	enemy_->Update();

	if (ImGui::BeginTabBar("camera"))
	{
		if (ImGui::BeginTabItem("Camera"))
		{
			ImGui::DragFloat3("translate", &viewProjection_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.01f);


			ImGui::EndTabItem();
		}
		ImGui::EndTabItem();
	}
	viewProjection_.translation_.y = Lerp(viewProjection_.translation_.y, player_->GetWorldPosition().y, 0.05f);
	viewProjection_.UpdateMatrix();

	collisoinManager->CheckAllCollisions();
}

void GameScene::Draw()
{
	/// -------描画処理開始-------

	/// 3Dオブジェクトの描画準備
	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----
	/*screw_->Draw(viewProjection_);*/
	player_->Draw(viewProjection_);
	enemy_->Draw(viewProjection_);

	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------

	//-----------------------------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----

	//------------------------



	/// ----------------------------------
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Draw();
#endif // _DEBUG
	/// -------描画処理終了-------
}
