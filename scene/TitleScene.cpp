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

	wt1_.Initialize();
	wt2_.Initialize();

	wt1_.translation_ = { -2.0f,0.0f,0.0f };
	wt2_.translation_ = { 2.0f,0.0f,0.0f };

	suzannu_ = std::make_unique<Object3d>();
	suzannu_->Initialize("walk.gltf");
	sphere_ = std::make_unique<Object3d>();
	sphere_->Initialize("debug/sphere.obj");

	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize("test", "debug/cube.obj");

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

	wt1_.UpdateMatrix();
	wt2_.UpdateMatrix();
}

void TitleScene::Draw()
{
	/// -------描画処理開始-------

	emitter_->DrawEmitter();

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----

	//------------------------

	objCommon_->skinningDrawCommonSetting();
	//-----アニメーションの描画開始-----
	suzannu_->Draw(wt1_, vp_);
	suzannu_->DrawSkelton(wt1_, vp_);
	//------------------------------

	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----
	sphere_->Draw(wt2_, vp_);
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	emitter_->Draw();
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

	objCommon_->skinningDrawCommonSetting();
	//-----アニメーションの描画開始-----
	
	//------------------------------

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


void TitleScene::Debug()
{
	ImGui::Begin("TitleScene:Debug");
	if (ImGui::BeginTabBar("1")) {
	debugCamera_->imgui();
	LightGroup::GetInstance()->imgui();
	ImGui::EndTabBar();
	}
	ImGui::End();
	emitter_->imgui();
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
