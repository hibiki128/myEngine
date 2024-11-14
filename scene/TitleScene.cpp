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

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("spaceStart.png", { 640.0f, 360.0f
		}, { 1,1,1,1 }, { 0.5f,0.5f });
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->ChangeScene("GAME");
	}
	//----------------------

	vP_.UpdateMatrix();
}

void TitleScene::Draw()
{
	/// -------描画処理開始-------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----
	sprite_->Draw();
	//------------------------

	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----

	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------

	//-----------------------------


	/// ----------------------------------
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Draw();
#endif // _DEBUG
	/// -------描画処理終了-------
}