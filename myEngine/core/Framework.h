#pragma once
#include"D3DResourceLeakChecker.h"
#include"d3dx12.h"
#include"DirectXCommon.h"
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
#include"Audio.h"
#include"SceneManager.h"
#include"AbstractSceneFactory.h"
#include"ParticleCommon.h"
#include"CollisionManager.h"
class Framework
{
public:// メンバ関数

	virtual ~Framework() = default;

	/// <summary>
	/// 実行
	/// </summary>
	void Run();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 終了チェック
	/// </summary>
	/// <returns></returns>
	virtual bool IsEndRequest() { return endRequest_; }

private:
	/// <summary>
	///  FPS表示
	/// </summary>
	void DisplayFPS();

protected:

	Input* input = nullptr;
	Audio* audio = nullptr;
	SceneManager* sceneManager_ = nullptr;
	SpriteCommon* spriteCommon = nullptr;
	Object3dCommon* object3dCommon = nullptr;
	ParticleCommon* particleCommon = nullptr;
	DirectXCommon* dxCommon = nullptr;
	WinApp* winApp;
	SrvManager* srvManager=nullptr;
	bool endRequest_;
	// シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
	std::unique_ptr<CollisionManager> collisionManager_;
};

