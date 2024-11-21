#pragma once
#include"Sprite.h"
#include"memory"
class SceneTransition
{
public:
	// フェードの状態
	enum class Status {
		None,    // フェードなし
		FadeIn,  // フェードイン中
		FadeOut, // フェードアウト中
	};

	SceneTransition();
	~SceneTransition();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// フェード開始
	/// </summary>
	void Start(Status status, float duration);

	/// <summary>
	/// フェード終了
	/// </summary>
	void Stop();

	/// <summary>
	/// フェード終了判定
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const;

private:
	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;

	// 現在のフェードの状態
	Status status_ = Status::None;

	std::unique_ptr<Sprite> sprite_ = nullptr;

	uint32_t texture = 0u;

};

