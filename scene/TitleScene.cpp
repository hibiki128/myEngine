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

	fall_ = std::make_unique<ParticleEmitter>();
	fall_->Initialize("fall", "game/smoke.obj");

	Break_ = std::make_unique<ParticleEmitter>();
	Break_->Initialize("break", "game/star.obj");
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
	ImGui::Begin("TitleScene");

	if (ImGui::BeginTabBar("1")) {
		if (ImGui::BeginTabItem("camera")) {
			ImGui::DragFloat3("translation", &vP_.translation_.x, 0.1f);
			ImGui::SliderAngle("rotateX", &vP_.rotation_.x);
			ImGui::SliderAngle("rotateY", &vP_.rotation_.y);
			ImGui::SliderAngle("rotateZ", &vP_.rotation_.z);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	if (ImGui::BeginTabBar("2")) {
		if (ImGui::BeginTabItem("fall")) {
			if (ImGui::Button("Update")) {
				fall_->SetActive(false);
			}
			fall_->RenderImGui();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("break")) {
			if (ImGui::Button("Update")) {
				Break_->SetActive(false);
			}
			Break_->RenderImGui();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("GAME");
	}
	//----------------------
	fall_->UpdateOnce(vP_);
	Break_->UpdateOnce(vP_);
	vP_.UpdateMatrix();
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
	fall_->DrawEmitter(vP_);
	Break_->DrawEmitter(vP_);
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	Break_->Draw();
	ptCommon_->SetBlendMode(BlendMode::kNormal);
	fall_->Draw();
	//-----------------------------

	/// -------描画処理終了-------
}