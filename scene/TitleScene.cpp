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

	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize("death", "debug/cube.obj");
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
		if (ImGui::BeginTabItem("particle")) {
			if (ImGui::Button("Update")) {
				emitter_->SetActive(false);
			}
			emitter_->RenderImGui();
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
	emitter_->SetValue();
	emitter_->UpdateOnce(vP_);
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
	emitter_->DrawEmitter(vP_);
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	emitter_->Draw();
	//-----------------------------

	/// -------描画処理終了-------
}