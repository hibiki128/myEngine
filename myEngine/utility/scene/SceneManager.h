#pragma once
#include"AbstractSceneFactory.h"
#include"memory"
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
	// 今のシーン(実行中のシーン)
	BaseScene* scene_ = nullptr;
	// 次のシーン
	BaseScene* nextScene_ = nullptr;
	// シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
};

