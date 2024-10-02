#include "GameScene.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include"SceneManager.h"
#include"TitleScene.h"

void GameScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	input_ = Input::GetInstance();

	viewProjection.Initialize();
	viewProjection.rotation_ = { 0.3f,0.0f,0.0f };
	viewProjection.translation_ = { 0.0f,4.0f,-10.0f };

	modelFilePath[0] = "axis.obj";
	modelFilePath[1] = "plane.obj";
	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel(modelFilePath[0]);
	ModelManager::GetInstance()->LoadModel(modelFilePath[1]);

	///----------Object3d----------
	object3d[0] = std::make_unique<Object3d>();
	object3d[1] = std::make_unique<Object3d>();
	object3d[0]->Initialize(modelFilePath[0]);
	object3d[1]->Initialize(modelFilePath[1]);
	///----------------------------

	///----------Sprite------------
	for (uint32_t i = 0; i < 1; ++i) {
		auto sprite = std::make_unique<Sprite>();
		std::string textureFilePath = "uvChecker.png";
		sprite->Initialize(textureFilePath, { 0.0f,0.0f });
		sprites.push_back(std::move(sprite)); // unique_ptrをvectorに移動
	}
	///----------------------------

	handle = audio_->LoadWave("fanfare.wav"); // handleの初期化をここに移動

	positions = {
		{100, 100}
	};

	modelWorldTransform[0].Initialize();
	modelWorldTransform[1].Initialize();
	modelWorldTransform[0].translation_ = { 0.0f,1.0f,1.0f };
	modelWorldTransform[1].translation_ = {3.0f, 1.0f, 1.0f};
}

void GameScene::Finalize()
{
	audio_->Unload(handle);
}

void GameScene::Update()
{

	modelWorldTransform[0].rotation_ = object3d[0]->GetRotation();
	modelWorldTransform[0].rotation_.y += 0.01f;
	object3d[0]->SetRotation(modelWorldTransform[0].rotation_);

	for (size_t i = 0; i < sprites.size(); ++i) {
		if (i < positions.size()) {
			sprites[i]->SetPosition(positions[i]);
		}
	}

	for (auto& sprite : sprites) {
		sprite->SetSize({ 256, 256 });
		sprite->Update();
	}

	if (input_->TriggerKey(DIK_0)) {
		audio_->StopWave(handle);
	}
	if (input_->TriggerKey(DIK_1)) {
		audio_->PlayWave(handle, 0.3f);
	}

	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("TITLE");
	}

#ifdef _DEBUG
	ImGuiManager::GetInstance()->Begin();
	ImGui::SliderFloat2("position", &positions[0].x, 0.0f, 1200.0f, "%4.1f");
	ImGuiManager::GetInstance()->End();
#endif // _DEBUG
	modelWorldTransform[0].UpdateMatrix();
	modelWorldTransform[1].UpdateMatrix();
	viewProjection.UpdateMatrix();

}

void GameScene::Draw()
{
	/// -------描画処理開始-------


	// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィクスコマンドを積む
	objCommon_->DrawCommonSetting();

	object3d[0]->Draw(modelWorldTransform[0],viewProjection);
	object3d[1]->Draw(modelWorldTransform[1], viewProjection);

	///----------スプライトの描画-----------
	// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
	spCommon_->DrawCommonSetting();

	for (auto& sprite : sprites) {
		sprite->Draw();
	}

	/// ----------------------------------
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Draw();
#endif // _DEBUG
	/// -------描画処理終了-------
}
