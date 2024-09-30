#include"D3DResourceLeakChecker.h"
#include"DirectXCommon.h"
#include"d3dx12.h"
#include"DirectXTex.h"
#ifdef _DEBUG
#include"ImGuiManager.h"
#endif // _DEBUG
#include"Input.h"
#include"ModelManager.h"
#include"Object3d.h"
#include"Object3dCommon.h"
#include"Sprite.h"
#include"SpriteCommon.h"
#include"SrvManager.h"
#include "StringUtility.h"
#include"TextureManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	D3DResourceLeakChecker();

	///---------WinApp--------
	WinApp* winApp = nullptr;
	// WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();
	///-----------------------

	///---------DirectXCommon----------
	DirectXCommon* dxCommon = nullptr;
	// DirectXCommonの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);
	///--------------------------------

	/// ---------ImGui---------
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Initialize(winApp, dxCommon);
#endif // _DEBUG
	/// -----------------------

	///--------SRVManager--------
	SrvManager* srvManager = nullptr;
	// SRVマネージャの初期化
	srvManager = new SrvManager();
	srvManager->Initialize(dxCommon);
	///--------------------------

	///----------Input-----------
	Input* input = nullptr;
	// 入力の初期化
	input = new Input();
	input->Initialize(winApp);
	///--------------------------

	///-----------TextureManager----------
	TextureManager::GetInstance()->Initialize(dxCommon, srvManager);
	///-----------------------------------

	///-----------ModelManager------------
	std::string modelFilePath[2];
	modelFilePath[0] = "axis.obj";
	modelFilePath[1] = "plane.obj";
	ModelManager::GetInstance()->Initialize(dxCommon, srvManager);
	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel(modelFilePath[0]);
	///----------------------------------

	///----------SpriteCommon------------
	SpriteCommon* spriteCommon = nullptr;
	// スプライト共通部の初期化
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);
	///----------------------------------

	///----------Object3dCommon-----------
	Object3dCommon* object3dCommon = nullptr;
	// 3Dオブジェクト共通部の初期化
	object3dCommon = new Object3dCommon;
	object3dCommon->Initialize(dxCommon);
	///-----------------------------------

	///---------Camera-------------
	Camera* camera = new Camera();
	camera->SetRotate({ 0.3f,0.0f,0.0f });
	camera->SetTranslate({ 0.0f,4.0f,-10.0f });
	object3dCommon->SetDefaultCamera(camera);
	///----------------------------

	///----------Object3d----------
	Object3d* object3d[2];
	object3d[0] = new Object3d;
	object3d[1] = new Object3d;
	object3d[0]->Initialize(object3dCommon);
	object3d[0]->SetModel(modelFilePath[0]);
	///----------------------------

	ModelManager::GetInstance()->LoadModel(modelFilePath[1]);
	object3d[1]->Initialize(object3dCommon);
	object3d[1]->SetModel(modelFilePath[1]);

	std::vector<Sprite*>sprites;
	///----------Sprite------------
	for (uint32_t i = 0; i < 5; ++i) {
		Sprite* sprite = new Sprite();
		std::string textureFilePath;
		if (i == 0 || i == 2 || i == 4) {
			textureFilePath = "resources/images/uvChecker.png";
		}
		else {
			textureFilePath = "resources/images/monsterBall.png";
		}
		sprite->Initialize(spriteCommon, textureFilePath);
		sprites.push_back(sprite);
	}
	///----------------------------

	std::vector<Vector2> positions = {
	{0, 300},
	{240, 300},
	{480, 300},
	{720, 300},
	{960, 300}
	};

	std::vector<Vector3> Object3dpos = {
		{0.0f,1.0f,1.0f},
		{3.0f,1.0f,1.0f},
	};

	float rotate;
	Vector4 color;
	/*Vector3 cameraRotate;*/

	Vector3 objRotate;


	/// ---------ゲームループ---------
	while (true) {
		// Windowsのメッセージ処理
		if (winApp->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}

		/// -------更新処理開始----------
#ifdef _DEBUG
		ImGuiManager::GetInstance()->Begin();
#endif // _DEBUG

		// -------Input-------
		// 入力の更新
		input->Update();
		if (input->TriggerKey(DIK_0)) {
			OutputDebugStringA("文字列リテラルを出力するよ\n");

			std::string a("stringに埋め込んだ文字列を出力するよ\n");
			OutputDebugStringA(a.c_str());
		}
		// -------------------

#ifdef _DEBUG
		ImGui::ShowDemoWindow();
		ImGuiManager::GetInstance()->End();
#endif // _DEBUG

		/// -------更新処理終了----------

		/// -------描画処理開始-------
		// 描画前処理
		dxCommon->PreDraw();
		srvManager->PreDraw();

		/*cameraRotate - camera->GetRotate();
		cameraRotate.y +- 0.005f;
		camera->SetRotate({ cameraRotate.x, cameraRotate.y,cameraRotate.z });*/
		camera->Update();


		// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィクスコマンドを積む
		object3dCommon->DrawCommonSetting();


		object3d[0]->SetPosition(Object3dpos[0]);
		object3d[1]->SetPosition(Object3dpos[1]);

		objRotate = object3d[0]->GetRotation();
		objRotate.y += 0.01f;
		object3d[0]->SetRotation(objRotate);

		object3d[0]->Update();
		object3d[1]->Update();

		//object3d[0]->Draw();
		//object3d[1]->Draw();

		// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
		spriteCommon->DrawCommonSetting();

		for (size_t i = 0; i < sprites.size(); ++i) {
			rotate = sprites[1]->GetRotation();
			color = sprites[2]->GetColor();
			rotate += 0.001f;
			color.x += 0.001f;
			if (color.x > 1.0f) {
				color.x -= 1.0f;
			}
			if (i < positions.size()) {
				sprites[i]->SetPosition(positions[i]);
			}
			sprites[0]->SetFlipX(true);
			sprites[1]->SetRotation(rotate);
			sprites[1]->SetTexturePath("resources/images/uvChecker.png");
			sprites[1]->SetFlipY(true);
			sprites[2]->SetColor(color);
			sprites[2]->SetAnchorPoint(Vector2{ -0.5f, -0.5f });
			sprites[4]->SetTexLeftTop(Vector2{ 0.0f, 0.0f });
			sprites[4]->SetTexSize(Vector2{ 64.0f,64.0f });
		}

		for (auto& sprite : sprites) {
			sprite->SetSize({ 128, 128 });
			sprite->Update();
		}

		///----------スプライトの描画-----------

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

	/// --------ループ終了---------

	// WindowsAPIの終了処理
	winApp->Finalize();

	/// -------TextureManager-------
	TextureManager::GetInstance()->Finalize();
	///-----------------------------

	/// -------ModelCommon-------
	ModelManager::GetInstance()->Finalize();
	///--------------------------
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Finalize();
#endif // _DEBUG


	delete object3d[0];
	delete object3d[1];

	// 解放処理
	for (auto& sprite : sprites) {
		delete sprite;
	}
	delete object3dCommon;
	delete spriteCommon;
	delete srvManager;
	delete dxCommon;
	delete input;
	delete winApp;

	return 0;
}
