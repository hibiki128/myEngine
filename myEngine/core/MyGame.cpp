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
	srvManager->PreDraw();
	dxCommon->PreRenderTexture();


	// -----描画開始-----
	object3dCommon->DrawCommonSetting();
	collisionManager_->Draw(*sceneManager_->GetBaseScene()->GetViewProjection());
	sceneManager_->Draw();
	// ------------------------

	dxCommon->PreDraw();
	offscreen_->Draw();

#ifdef _DEBUG
	ImGuiManager::GetInstance()->Draw();
#endif // _DEBUG

	// -----描画終了-----
	dxCommon->PostDraw();
}