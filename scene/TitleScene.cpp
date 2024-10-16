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
	line_ = std::make_unique<LineManager>();
	line_->Initialize(SrvManager::GetInstance());
	line_->CreateParticleGroup("line", "plane.obj");
	vP_.Initialize();
	// startとendの初期化
	start.resize(1); // 要素数10のベクトルをリサイズ
	end.resize(1);   // 要素数10のベクトルをリサイズ

	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);
	debugCamera_->Initialize();

	for (int i = 0; i < 1; ++i) {
		start[i] = Vector3(0.0f + i, 0.0f, 0.0f); // startのxを+1ずつ増やす
		end[i] = Vector3(0.5f + i, 0.5f + i, 0.0f);   // endのxを+1ずつ増やす
	}
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
	move = input_->GetWheel();
	ImGui::Begin("camera");
	ImGui::DragFloat3("translate", &vP_.translation_.x, 0.1f);
	ImGui::Text("isDebugCamera %s", isDebugCameraActive_ ? "true" : "false");
	ImGui::Text("move %d", move);
	ImGui::End();

	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("GAME");
	}
	//----------------------

	if (input_->TriggerKey(DIK_F1)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}

	if (isDebugCameraActive_) {
		debugCamera_->Update();
	/*	vP_.matView_ = debugCamera_->GetViewProjection().matView_;
		vP_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;*/
		vP_.translation_ = debugCamera_->GetViewProjection().translation_;
		vP_.rotation_ = debugCamera_->GetViewProjection().rotation_;
		vP_.TransferMatrix();
	}
	else {
		vP_.UpdateMatrix();
	}

	line_->Update(vP_, start, end);
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
	line_->Draw();
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
