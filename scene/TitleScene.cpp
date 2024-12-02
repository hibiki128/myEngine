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
	wt1_.Initialize();
	wt2_.Initialize();

	wt1_.translation_ = { -2.0f,0.0f,0.0f };
	wt2_.translation_ = { 2.0f,0.0f,0.0f };

	suzannu_ = std::make_unique<Object3d>();
	suzannu_->Initialize("debug/suzannu.obj");
	sphere_ = std::make_unique<Object3d>();
	sphere_->Initialize("debug/sphere.obj");

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vP_);
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{

	ImGui::Begin("scene"); 
	debugCamera_->imgui();
	ImGui::End();

	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("GAME");
	}
	//----------------------
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	}
	else {
	vP_.UpdateMatrix();
	}

	wt1_.UpdateMatrix();
	wt2_.UpdateMatrix();
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
	suzannu_->Draw(wt1_, vP_);
	sphere_->Draw(wt2_, vP_);
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------

	//-----------------------------

	/// -------描画処理終了-------
}