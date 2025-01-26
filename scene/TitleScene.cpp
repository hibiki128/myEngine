#include "TitleScene.h"
#include "ImGuiManager.h"
#include"SceneManager.h"
#include"SrvManager.h"

#ifdef _DEBUG
#include<imgui.h>
#endif // _DEBUG
#include <LightGroup.h>
#include"line/DrawLine3D.h"

void TitleScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();
	vp_.Initialize();
	vp_.translation_ = { 0.0f,0.0f,-30.0f };

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);

	emitter_ = std::make_unique<ParticleEmitter>();
	emitter2_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize("reverse", "debug/sphere.obj");
	emitter_->SetTexture("white1x1.png");
	emitter2_->Initialize("reverse_black", "debug/sphere.obj");
	emitter2_->SetTexture("black1x1.png");

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("debug/cube.obj");
	obj2_ = std::make_unique<Object3d>();
	obj2_->Initialize("debug/cube.obj");
	wt_.Initialize();
	wt2_.Initialize();
	wt2_.translation_ = { 5.0f,0.0f,0.0f };
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
#ifdef _DEBUG
	// デバッグ
	Debug();
#endif // _DEBUG

	// カメラ更新
	CameraUpdate();

	// シーン切り替え
	ChangeScene();

	emitter_->imgui();
	emitter2_->imgui();

	ImGui::Begin("パーティクル");
	if (ImGui::Button("生成")) {
		emitter_->UpdateOnce();
	}
	if (ImGui::Button("生成2")) {
		emitter2_->UpdateOnce();
	}
	if (isAuto_) {
		emitter_->Update();
		emitter2_->Update();
	}
	ImGui::End();

	wt_.UpdateMatrix();
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
	emitter_->DrawEmitter();
	emitter2_->DrawEmitter();
	obj_->Draw(wt_, vp_);
	obj2_->Draw(wt2_, vp_);
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	ptCommon_->SetBlendMode(BlendMode::kNormal);
	emitter_->Draw(vp_);
	emitter2_->Draw(vp_);
	//-----------------------------

	//-----線描画-----
	DrawLine3D::GetInstance()->Draw(vp_);
	//---------------

	/// ----------------------------------

	/// -------描画処理終了-------
}

void TitleScene::DrawForOffScreen()
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


	/// ----------------------------------

	/// -------描画処理終了-------
}


void TitleScene::Debug()
{
	ImGui::Begin("TitleScene:Debug");
	ImGui::Checkbox("パーティクルの自動更新", &isAuto_);
	if (ImGui::Button("変更")) {
		obj_->SetTexture("monsterBall.png");
	}

	debugCamera_->imgui();
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
