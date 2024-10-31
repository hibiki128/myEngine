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

	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize("emi0","plane.obj");

	vp_.Initialize();
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{

	ImGui::Begin("scene");
	if(ImGui::BeginTabBar("camera")) {
		if (ImGui::BeginTabItem("camera")) {
			ImGui::DragFloat3("translation", &vp_.translation_.x, 0.1f);
			ImGui::DragFloat3("rotation", &vp_.rotation_.x, 0.1f);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	emitter_->RenderImGui();

	ImGui::End();

	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("GAME");
	}

	vp_.UpdateMatrix();

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
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Draw();
#endif // _DEBUG
	/// -------描画処理終了-------
}
