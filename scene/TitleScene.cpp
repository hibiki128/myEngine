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

	LightGroup::GetInstance()->Initialize();

	vp_.Initialize();
	wt_.Initialize();

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("debug/suzannu.obj");
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{

	ImGui::Begin("titleScene");
	debugCamera_->imgui();
	LightGroup::GetInstance()->imgui();
	ImGui::End();

	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->NextSceneReservation("GAME");
	}
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	}
	else {
		vp_.UpdateMatrix();
	}
	wt_.UpdateMatrix();
}

void TitleScene::Draw()
{
	/// -------描画処理開始-------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----

	//------------------------

	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----
	obj_->Draw(wt_, vp_);
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------

	//-----------------------------

	/// -------描画処理終了-------
}