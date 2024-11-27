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

	for (int i = 0; i < 3; i++) {
		std::unique_ptr<ParticleEmitter> emitter_;
		emitter_ = std::make_unique<ParticleEmitter>();
		if (i == 0) {
			emitter_->Initialize("core1", "game/coreFraction1.obj");
		}
		else if (i == 1) {
			emitter_->Initialize("core2", "game/coreFraction2.obj");
		}
		else if (i == 2) {
			emitter_->Initialize("core3", "game/coreFraction3.obj");
		}

		coreFractions_.push_back(std::move(emitter_));
	}

	fall_ = std::make_unique<ParticleEmitter>();
	fall_->Initialize("fall", "game/smoke.obj");

	Break_ = std::make_unique<ParticleEmitter>();
	Break_->Initialize("break", "game/star.obj");

	WeekBreak_ = std::make_unique<ParticleEmitter>();
	WeekBreak_->Initialize("WeekBreak", "debug/Triangle2D.obj");

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("debug/suzannu.obj");
	obj2_ = std::make_unique<Object3d>();
	obj2_->Initialize("debug/ICO.obj");

	wt_.Initialize();
	wt2_.Initialize();
	wt2_.translation_ = { 3.0f,0.0f,0.0f };

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vP_);
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
	ImGui::Begin("TitleScene");

	if (ImGui::BeginTabBar("1")) {
		if (ImGui::BeginTabItem("camera")) {
			debugCamera_->imgui();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	//if (ImGui::BeginTabBar("2")) {
	//	if (ImGui::BeginTabItem("fall")) {
	//		if (ImGui::Button("Update")) {
	//			fall_->SetActive(false);
	//		}
	//		fall_->RenderImGui();
	//		ImGui::EndTabItem();
	//	}
	//	if (ImGui::BeginTabItem("break")) {
	//		if (ImGui::Button("Update")) {
	//			Break_->SetActive(false);
	//		}
	//		Break_->RenderImGui();
	//		ImGui::EndTabItem();
	//	}
	//	if (ImGui::BeginTabItem("WeekBreak")) {
	//		if (ImGui::Button("Update")) {
	//			WeekBreak_->SetActive(false);
	//		}
	//		WeekBreak_->RenderImGui();
	//		ImGui::EndTabItem();
	//	}
	//	for (size_t i = 0; i < coreFractions_.size(); ++i) {
	//		// 各ParticleEmitterごとにタブを作成
	//		std::string tabName = "CoreFraction " + std::to_string(i + 1);
	//		if (ImGui::BeginTabItem(tabName.c_str())) {
	//			// 更新ボタン
	//			if (ImGui::Button(("Update##" + std::to_string(i)).c_str())) {
	//				// ParticleEmitterを無効化（仮にSetActiveを用いるとします）
	//				coreFractions_[i]->SetActive(false);
	//			}

	//			// ImGuiによるプロパティやデバッグ情報のレンダリング
	//			coreFractions_[i]->RenderImGui();

	//			ImGui::EndTabItem();
	//		}
	//	}
	//	if(ImGui::BeginTabItem("AllCore")) {
	//		if (ImGui::Button("Update")) {
	//			for (auto& coreFraction_ : coreFractions_) {
	//				coreFraction_->SetActive(false);
	//			}
	//		}
	//		ImGui::EndTabItem();
	//	}
	//	ImGui::EndTabBar();
	//}
	ImGui::End();

	LightGroup::GetInstance()->imgui();

	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->NextSceneReservation("GAME");
	}
	//----------------------
	/*fall_->UpdateOnce(vP_);
	Break_->UpdateOnce(vP_);
	WeekBreak_->UpdateOnce(vP_);
	for (auto& coreFraction_ : coreFractions_) {
		coreFraction_->UpdateOnce(vP_);
	}*/
	debugCamera_->Update();
	if (!debugCamera_->GetActive()) {
		vP_.UpdateMatrix();
	}
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
	/*obj_->Draw(wt_, vP_);
	obj2_->Draw(wt2_, vP_);
	fall_->DrawEmitter(vP_);
	Break_->DrawEmitter(vP_);
	WeekBreak_->DrawEmitter(vP_);
	for (auto& coreFraction_ : coreFractions_) {
		coreFraction_->DrawEmitter(vP_);
	}*/

	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	/*Break_->Draw();
	fall_->Draw();
	WeekBreak_->Draw();
	for (auto& coreFraction_ : coreFractions_) {
		coreFraction_->Draw();
	}*/
	//-----------------------------

	/// -------描画処理終了-------
}