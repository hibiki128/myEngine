#include "SceneManager.h"
#include <cassert>
#include <ImGuiManager.h>
#include"GlobalVariables.h"

SceneManager* SceneManager::instance = nullptr;

SceneManager* SceneManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new SceneManager;
	}
	return instance;
}

void SceneManager::Initialize()
{
	transition_ = std::make_unique<SceneTransition>();
	transition_->Initialize();
}

void SceneManager::Finalize()
{
	scene_->Finalize();
	delete scene_;
	delete instance;
	instance = nullptr;
}

void SceneManager::Update()
{
	ImGui::Begin("scene");
	if (ImGui::Button("TitleScene")) {
		nextScene_ = sceneFactory_->CreateScene("TITLE");
	}
	if (ImGui::Button("GameScene")) {
		nextScene_ = sceneFactory_->CreateScene("GAME");
	}
	ImGui::End();

	// 次のシーンの予約があるなら
	if (nextScene_) {
		SceneChange();
	}

	// 実行中シーンを更新する
	scene_->Update();
}

void SceneManager::Draw()
{
	scene_->Draw();
}

void SceneManager::NextSceneReservation(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	
	// 次シーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

void SceneManager::SceneChange()
{
	// 旧シーンの終了
	if (scene_) {
		scene_->Finalize();
		delete scene_;
	}
	// シーンの切り替え
	scene_ = nextScene_;
	nextScene_ = nullptr;

	// シーンマネージャをセット
	scene_->SetSceneManager(this);

	// 次のシーンを初期化する
	scene_->Initialize();
}
