#include "Framework.h"

void Framework::Run()
{
	// ゲームの初期化
	Initialize();

	while (true) // ゲームループ
	{
		// 更新
		Update();
		// 終了リクエストが来たら抜ける
		if (IsEndRequest()) {
			break;
		}
		// 描画
		Draw();
	}
	// ゲームの終了
	Finalize();
}

void Framework::Initialize()
{

	D3DResourceLeakChecker();

	///---------WinApp--------
	// WindowsAPIの初期化
	winApp = std::make_unique<WinApp>();
	winApp->Initialize();
	///-----------------------

	///---------DirectXCommon----------
	// DirectXCommonの初期化
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(winApp.get());
	///--------------------------------

	/// ---------ImGui---------
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Initialize(winApp.get());
#endif // _DEBUG
	/// -----------------------

	///--------SRVManager--------
	// SRVマネージャの初期化
	srvManager = SrvManager::GetInstance();
	srvManager->Initialize();
	///--------------------------

	///----------Input-----------
	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize(winApp.get());
	///--------------------------

	///-----------TextureManager----------
	TextureManager::GetInstance()->Initialize(srvManager);
	///-----------------------------------

	///-----------ModelManager------------
	ModelManager::GetInstance()->Initialize(srvManager);
	///----------------------------------

	///----------SpriteCommon------------
	// スプライト共通部の初期化
	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Initialize();
	///----------------------------------

	///----------Object3dCommon-----------
	// 3Dオブジェクト共通部の初期化
	object3dCommon = Object3dCommon::GetInstance();
	object3dCommon->Initialize();
	///-----------------------------------

	///----------ParticleCommon------------
	particleCommon = ParticleCommon::GetInstance();
	particleCommon->Initialize(dxCommon);
	///------------------------------------

	///---------Audio-------------
	audio = Audio::GetInstance();
	audio->Initialize();

	sceneManager_ = SceneManager::GetInstance();
	
}

void Framework::Finalize()
{
	sceneManager_->Finalize();

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
	srvManager->Finalize();
	audio->Finalize();
	input->Finalize();
	object3dCommon->Finalize();
	spriteCommon->Finalize();
	particleCommon->Finalize();
	dxCommon->Finalize();
	delete sceneFactory_;
}

void Framework::Update()
{
	sceneManager_->Update();
	/// -------更新処理開始----------
	
	// -------Input-------
	// 入力の更新
	input->Update();
	// -------------------

	/// -------更新処理終了----------
	endRequest_ = winApp->ProcessMessage();
}

void Framework::Draw()
{

}
