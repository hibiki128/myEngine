#include "TitleScene.h"
#include "ImGuiManager.h"
#include"SceneManager.h"
#include"SrvManager.h"
#include"Model.h"

void TitleScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();

	vp_.Initialize();

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("sneakWalk.gltf");
	wt_.Initialize();
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("TitleScene");
	if (ImGui::BeginTabBar("camera")) {
		if (ImGui::BeginTabItem("camera")) {
			ImGui::DragFloat3("translation", &vp_.translation_.x, 0.1f);
			ImGui::SliderAngle("X", &vp_.rotation_.x);
			ImGui::SliderAngle("Y", &vp_.rotation_.y);
			ImGui::SliderAngle("Z", &vp_.rotation_.z);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	if (ImGui::BeginTabBar("transform")) {
		if (ImGui::BeginTabItem("transform")) {
			ImGui::DragFloat3("translation", &wt_.translation_.x, 0.1f);
			ImGui::SliderAngle("X", &wt_.rotation_.x);
			ImGui::SliderAngle("Y", &wt_.rotation_.y);
			ImGui::SliderAngle("Z", &wt_.rotation_.z);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
#endif // _DEBUG

	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("GAME");
	}
	//----------------------
	vp_.UpdateMatrix();
	wt_.UpdateMatrix();
}

void TitleScene::Draw()
{
	/// -------描画処理開始-------
	
	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----
	
	//------------------------

	objCommon_->skinningDrawCommonSetting();
	obj_->Draw(wt_, vp_);

	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----
	obj_->DrawSkelton(wt_,vp_);
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	
	//-----------------------------


	/// -------描画処理終了-------
}