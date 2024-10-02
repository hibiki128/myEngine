#include "MyGame.h"
#include"SceneFactory.h"

void MyGame::Initialize()
{
	Framework::Initialize();

	// -----ゲーム固有の処理-----

	// 最初のシーンの生成
	sceneFactory_ = new SceneFactory();
	// シーンマネージャに最初のシーンをセット
	sceneManager_->SetSceneFactory(sceneFactory_);
	sceneManager_->ChangeScene("TITLE");
	// -----------------------

}

void MyGame::Finalize()
{
	// -----ゲーム固有の処理-----
	
	// -----------------------

	Framework::Finalize();
}

void MyGame::Update()
{
	Framework::Update();
	// -----ゲーム固有の処理-----

	// -----------------------
	
}

void MyGame::Draw()
{
	dxCommon->PreDraw();
	srvManager->PreDraw();
	// -----描画開始-----


	// -----シーンごとの処理------
	
	sceneManager_->Draw();

	// ------------------------


	// -----描画終了-----
	dxCommon->PostDraw();
}