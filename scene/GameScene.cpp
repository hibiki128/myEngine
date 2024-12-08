#include "GameScene.h"
#include <LightGroup.h>
#include"SceneManager.h"

void GameScene::Finalize()
{

}

void GameScene::Initialize()
{

	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();

	vp_.Initialize();
	vp_.translation_ = { 0.0f,50.0f,-300.0f };

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);

	//-----地面-----
	ground_ = std::make_unique<Ground>();
	ground_->Initialize();
	//-------------

	//-----隕石-----

	//-------------
}

void GameScene::Update()
{
	// デバッグ
	Debug();

	// カメラ更新
	CameraUpdate();

	// シーン切り替え
	ChangeScene();
	GenerateMeteor();
	ground_->Update();
	for (auto& meteor : meteors_) {
		meteor->Update();
	}

	meteors_.erase(
		std::remove_if(meteors_.begin(), meteors_.end(),
			[](const std::unique_ptr<Meteor>& meteor) {
				return meteor->IsAlive();
			}),
		meteors_.end());
}

void GameScene::Draw()
{
	/// -------描画処理開始-------

	/// 3Dオブジェクトの描画準備
	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----
	ground_->Draw(vp_);
	for (auto& meteor : meteors_) {
		meteor->Draw(vp_);
	}
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	for (auto& meteor : meteors_) {
		meteor->DrawParticle(vp_);
	}
	//-----------------------------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始----

	//------------------------



	/// ----------------------------------

	/// -------描画処理終了-------
}

void GameScene::Debug()
{
	ImGui::Begin("GameScene:Debug");
	debugCamera_->imgui();
	LightGroup::GetInstance()->imgui();
	ImGui::End();

	ImGui::Begin("操作説明");
	ImGui::Text("Enterキーでインセキを落とします");
	ImGui::End();
}

void GameScene::CameraUpdate()
{
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	}
	else {
		vp_.UpdateMatrix();
	}
}

void GameScene::ChangeScene()
{
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->NextSceneReservation("TITLE");
	}
}

void GameScene::GenerateMeteor()
{
	if (input_->TriggerKey(DIK_RETURN)) {
		std::unique_ptr<Meteor> newMeteor;
		newMeteor = std::make_unique<Meteor>();
		newMeteor->Initialize();

		meteors_.push_back(std::move(newMeteor));
	}
}
