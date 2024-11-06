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

	vP_.Initialize();

	rail_ = std::make_unique<rail>();
	rail_->Initialize(&vP_);

	cameraT_.Initialize();

	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize(cameraT_);

	player_ = std::make_unique<Player>();
	Vector3 playerPos_ = { 0.0f,-0.37f,0.2f };
	player_->Initilaize(&vP_, playerPos_);
	player_->SetParent(&railCamera_->GetWorldTransform());
}

void GameScene::Finalize()
{

}

void GameScene::Update()
{

	ImGui::Begin("gameScene");
	if (ImGui::BeginTabBar("camera")) {
		if (ImGui::BeginTabItem("camera")) {
			ImGui::DragFloat3("translate", &vP_.translation_.x, 0.1f);
			ImGui::DragFloat3("rotate", &vP_.rotation_.x, 0.1f);
			ImGui::EndTabItem();
		}
		railCamera_->imgui();
		ImGui::EndTabBar();
	}
	player_->imgui();

	rail_->imgui();

	ImGui::End();


	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("TITLE");
	}
	//----------------------

	rail_->Update();


	vP_.UpdateMatrix();
	railCamera_->SetControlPoints(rail_->GetControlPoints());
	railCamera_->Update();
	vP_.matView_ = railCamera_->GetViewProjection().matView_;
	player_->SetRailCamera(railCamera_.get());
	vP_.TransferMatrix();
	player_->Update();
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
	rail_->IcoDraw();
	player_->Draw();
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	rail_->railDraw();
	//-----------------------------


	/// ----------------------------------
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Draw();
#endif // _DEBUG
	/// -------描画処理終了-------
}
