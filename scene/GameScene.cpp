#include "GameScene.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "ParticleManager.h"
#include <ModelManager.h>
#include"SceneManager.h"
#include"ModelManager.h"

void GameScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();

	vp_.Initialize();
}

void GameScene::Finalize()
{

}

void GameScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("gameScene");
	
	ImGui::End();
#endif // _DEBUG

	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->NextSceneReservation("TITLE");
	}

	//----------------------

	vp_.UpdateMatrix();
}

void GameScene::Draw()
{
	/// -------描画処理開始-------

		/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----
	
	//------------------------

	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----
	
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	
	//-----------------------------

	/// -------描画処理終了-------
}