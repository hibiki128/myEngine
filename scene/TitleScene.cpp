#include "TitleScene.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include"SceneManager.h"

void TitleScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	input_ = Input::GetInstance();

	///---------Camera-------------
	camera = std::make_unique<Camera>();
	camera->SetRotate({ 0.3f,0.0f,0.0f });
	camera->SetTranslate({ 0.0f,4.0f,-10.0f });
	objCommon_->SetDefaultCamera(camera.get());
	///----------------------------

	modelFilePath[0] = "axis.obj";
	modelFilePath[1] = "plane.obj";
	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel(modelFilePath[0]);
	ModelManager::GetInstance()->LoadModel(modelFilePath[1]);

	///----------Object3d----------
	object3d[0] = std::make_unique<Object3d>();
	object3d[1] = std::make_unique<Object3d>();
	object3d[0]->Initialize(objCommon_);
	object3d[0]->SetModel(modelFilePath[0]);
	///----------------------------

	object3d[1]->Initialize(objCommon_);
	object3d[1]->SetModel(modelFilePath[1]);

	///----------Sprite------------
	for (uint32_t i = 0; i < 1; ++i) {
		auto sprite = std::make_unique<Sprite>();
		std::string textureFilePath = "uvChecker.png";
		sprite->Initialize(spCommon_, textureFilePath);
		sprites.push_back(std::move(sprite)); // unique_ptrをvectorに移動
	}
	///----------------------------

	handle = audio_->LoadWave("fanfare.wav"); // handleの初期化をここに移動

	positions = {
		{100, 100}
	};

	Object3dpos = {
		{0.0f,1.0f,1.0f},
		{3.0f,1.0f,1.0f},
	};
}

void TitleScene::Finalize()
{
	audio_->Unload(handle);
}

void TitleScene::Update()
{

	camera->Update();

	object3d[0]->SetPosition(Object3dpos[0]);
	object3d[1]->SetPosition(Object3dpos[1]);

	objRotate = object3d[0]->GetRotation();
	objRotate.y += 0.01f;
	object3d[0]->SetRotation(objRotate);

	object3d[0]->Update();
	object3d[1]->Update();

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
		sceneManager_->ChangeScene("GAME");
	}

#ifdef _DEBUG
	ImGuiManager::GetInstance()->Begin();
	ImGui::SetWindowSize({ 500.0f, 100.0f });
	ImGui::SliderFloat2("position", &positions[0].x, 0.0f, 1200.0f, "%4.1f");
	ImGuiManager::GetInstance()->End();
#endif // _DEBUG


}

void TitleScene::Draw()
{
	/// -------描画処理開始-------


	// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィクスコマンドを積む
	objCommon_->DrawCommonSetting();

	object3d[0]->Draw();
	object3d[1]->Draw();

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
