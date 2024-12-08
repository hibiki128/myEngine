#include "TitleScene.h"
#include "ImGuiManager.h"
#include"SceneManager.h"
#include"SrvManager.h"

#ifdef _DEBUG
#include<imgui.h>
#endif // _DEBUG
#include <LightGroup.h>

void TitleScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);

	vp_.Initialize();
	
	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize("Particle", "debug/sphere.obj");

}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
	// デバッグ
	Debug();

	// カメラ更新
	CameraUpdate();

	// シーン切り替え
	ChangeScene();

	emitter_->Update(vp_);

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
	emitter_->DrawEmitter(vp_);
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	emitter_->Draw();
	//-----------------------------


	/// ----------------------------------

	/// -------描画処理終了-------
}


void TitleScene::Debug()
{
	ImGui::Begin("TitleScene:Debug");
	emitter_->imgui();
	debugCamera_->imgui();
	LightGroup::GetInstance()->imgui();
	ImGui::End();
}

void TitleScene::CameraUpdate()
{
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	}
	else {
		vp_.UpdateMatrix();
	}
}

void TitleScene::ChangeScene()
{
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->NextSceneReservation("GAME");
	}
}
