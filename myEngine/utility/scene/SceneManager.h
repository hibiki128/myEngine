#pragma once
#include"AbstractSceneFactory.h"
#include"memory"
#include"SceneTransition.h"
class SceneManager
{
private:
	static SceneManager* instance;

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator=(SceneManager&) = delete;

public:// メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static SceneManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

public: // setter
	/// <summary>
	/// シーンファクトリーのセット
	/// </summary>
	/// <param name="sceneFactory"></param>
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

	/// <summary>
	/// 次シーン予約
	/// </summary>
	/// <param name="nextScene"></param>
	void ChangeScene(const std::string& sceneName);

	BaseScene* GetBaseScene() { return scene_; }

private:
	// ゲームのフェーズ(型)
	enum class Phase {
		kFadeIn,
		kPlay,  // ゲームプレイ
		kFadeOut,
	};
	// 今のシーン(実行中のシーン)
	BaseScene* scene_ = nullptr;
	// 次のシーン
	BaseScene* nextScene_ = nullptr;
	// シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
	std::unique_ptr<SceneTransition> transition_;
	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

};

