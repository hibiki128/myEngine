#include "TitleScene.h"
#include "ImGuiManager.h"
#include"SceneManager.h"
#include"SrvManager.h"

void TitleScene::Initialize()
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

void TitleScene::Finalize()
{

}

void TitleScene::Update()
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
		sceneManager_->ChangeScene("GAME");
	}
	//----------------------

	player_->Update();

	rail_->Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		isActive_ = true;
	}

	if (isActive_) {
		railCamera_->SetControlPoints(rail_->GetControlPoints());
		railCamera_->SetPlayerPositionZ(player_->GetLocalPosition().z);
		vP_.UpdateMatrix();
		railCamera_->Update();
		vP_.matView_ = railCamera_->GetViewProjection().matView_;
		vP_.matProjection_ = railCamera_->GetViewProjection().matProjection_;
		vP_.matWorld_ = railCamera_->GetViewProjection().matWorld_;
		vP_.TransferMatrix();
	}
	else {
		vP_.UpdateMatrix();
	}
}

void TitleScene::Draw()
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