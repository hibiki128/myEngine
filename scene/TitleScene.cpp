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
	vp_.Initialize();
	line = std::make_unique<LineManager>();
	line->Initialize(SrvManager::GetInstance());
	line->CreateParticleGroup("name", "line.obj");
	start.push_back({ 0.0f,0.0f,0.0f });
	end.push_back({ 1.0f,1.0f,1.0f });
	screw_ = std::make_unique<Screw>();
	screw_->Initialize();
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{

	ImGui::Begin("camera");
	ImGui::DragFloat3("postion", &vp_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &vp_.rotation_.x, 0.1f);
	ImGui::End();

	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("GAME");
	}
	//----------------------

	screw_->Update();

	vp_.UpdateMatrix();
	line->Update(vp_, screw_->GetStartPoint(), screw_->GetEndPoint());

}

void TitleScene::Draw()
{
	/// -------描画処理開始-------


	/// 3Dオブジェクトの描画準備
	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----

	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------

	line->Draw();
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
