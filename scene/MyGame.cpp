#include "MyGame.h"

void MyGame::Init()
{
	D3DResourceLeakChecker();

	///---------WinApp--------
	// WindowsAPIの初期化
	winApp = std::make_unique<WinApp>();
	winApp->Initialize();
	///-----------------------

	///---------DirectXCommon----------
	// DirectXCommonの初期化
	dxCommon = std::make_unique<DirectXCommon>();
	dxCommon->Initialize(winApp.get());
	///--------------------------------

	/// ---------ImGui---------
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Initialize(winApp.get(), dxCommon.get());
#endif // _DEBUG
	/// -----------------------

	///--------SRVManager--------
	// SRVマネージャの初期化
	srvManager = std::make_unique<SrvManager>();
	srvManager->Initialize(dxCommon.get());
	///--------------------------

	///----------Input-----------
	// 入力の初期化
	input = std::make_unique<Input>();
	input->Initialize(winApp.get());
	///--------------------------

	///-----------TextureManager----------
	TextureManager::GetInstance()->Initialize(dxCommon.get(), srvManager.get());
	///-----------------------------------

	///-----------ModelManager------------
	modelFilePath[0] = "axis.obj";
	modelFilePath[1] = "plane.obj";
	ModelManager::GetInstance()->Initialize(dxCommon.get(), srvManager.get());
	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel(modelFilePath[0]);
	///----------------------------------

	///----------SpriteCommon------------
	// スプライト共通部の初期化
	spriteCommon = std::make_unique<SpriteCommon>();
	spriteCommon->Initialize(dxCommon.get());
	///----------------------------------

	///----------Object3dCommon-----------
	// 3Dオブジェクト共通部の初期化
	object3dCommon = std::make_unique<Object3dCommon>();
	object3dCommon->Initialize(dxCommon.get());
	///-----------------------------------

	///---------Camera-------------
	camera = std::make_unique<Camera>();
	camera->SetRotate({ 0.3f,0.0f,0.0f });
	camera->SetTranslate({ 0.0f,4.0f,-10.0f });
	object3dCommon->SetDefaultCamera(camera.get());
	///----------------------------

	///----------Object3d----------
	object3d[0] = std::make_unique<Object3d>();
	object3d[1] = std::make_unique<Object3d>();
	object3d[0]->Initialize(object3dCommon.get());
	object3d[0]->SetModel(modelFilePath[0]);
	///----------------------------

	ModelManager::GetInstance()->LoadModel(modelFilePath[1]);
	object3d[1]->Initialize(object3dCommon.get());
	object3d[1]->SetModel(modelFilePath[1]);

	///----------Sprite------------
	for (uint32_t i = 0; i < 1; ++i) {
		auto sprite = std::make_unique<Sprite>();
		std::string textureFilePath = "uvChecker.png";
		sprite->Initialize(spriteCommon.get(), textureFilePath);
		sprites.push_back(std::move(sprite)); // unique_ptrをvectorに移動
	}
	///----------------------------

	///---------Audio-------------
	audio = Audio::GetInstance();
	audio->Initialize();
	handle = audio->LoadWave("fanfare.wav"); // handleの初期化をここに移動

	positions = {
		{100, 100}
	};

	Object3dpos = {
		{0.0f,1.0f,1.0f},
		{3.0f,1.0f,1.0f},
	};
}

void MyGame::Finalize()
{
	// WindowsAPIの終了処理
	winApp->Finalize();

	/// -------TextureManager-------
	TextureManager::GetInstance()->Finalize();
	///-----------------------------

	/// -------ModelCommon-------
	ModelManager::GetInstance()->Finalize();
	///---------------------------

#ifdef _DEBUG
	ImGuiManager::GetInstance()->Finalize();
#endif // _DEBUG

	audio->Unload(handle);
	audio->Finalize();
}

void MyGame::Update()
{
	/// -------更新処理開始----------
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

	// -------Input-------
	// 入力の更新
	input->Update();
	if (input->TriggerKey(DIK_0)) {
		audio->StopWave(handle);
	}
	if (input->TriggerKey(DIK_1)) {
		audio->PlayWave(handle, 0.3f);
	}
	// -------------------

#ifdef _DEBUG
	ImGuiManager::GetInstance()->Begin();
	ImGui::SetWindowSize({ 500.0f, 100.0f });
	ImGui::SliderFloat2("position", &positions[0].x, 0.0f, 1200.0f, "%4.1f");
	ImGuiManager::GetInstance()->End();
#endif // _DEBUG

	/// -------更新処理終了----------
	endRequest_ = winApp->ProcessMessage();
}

void MyGame::Draw()
{
	/// -------描画処理開始-------
	// 描画前処理
	dxCommon->PreDraw();
	srvManager->PreDraw();

	// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィクスコマンドを積む
	object3dCommon->DrawCommonSetting();

	//object3d[0]->Draw();
	//object3d[1]->Draw();

	///----------スプライトの描画-----------
	// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
	spriteCommon->DrawCommonSetting();

	for (auto& sprite : sprites) {
		sprite->Draw();
	}

	/// ----------------------------------
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Draw();
#endif // _DEBUG
	// 描画後処理
	dxCommon->PostDraw();

	/// -------描画処理終了-------
}