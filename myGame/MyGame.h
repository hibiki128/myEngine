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
#include"myGame/class/GlobalVariables.h"
#include <memory> // スマートポインタのために追加

class MyGame
{
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

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

	/// <summary>
	/// 終了リクエスト
	/// </summary>
	/// <returns></returns>
	bool IsEndRequest() { return endRequest_; }

	ImGuiManager* GetimgManager() { return imgui; }

private:
	std::unique_ptr<WinApp> winApp; // unique_ptrに変更
	std::unique_ptr<DirectXCommon> dxCommon; // unique_ptrに変更
	std::unique_ptr<SrvManager> srvManager; // unique_ptrに変更
	std::unique_ptr<Input> input; // unique_ptrに変更
	std::string modelFilePath[2];
	std::unique_ptr<SpriteCommon> spriteCommon; // unique_ptrに変更
	std::unique_ptr<Object3dCommon> object3dCommon; // unique_ptrに変更
	std::unique_ptr<Camera> camera; // unique_ptrに変更
	std::unique_ptr<Object3d> object3d[2]; // unique_ptrに変更
	std::vector<std::unique_ptr<Sprite>> sprites; // unique_ptrに変更
	Audio* audio = nullptr;
	ImGuiManager* imgui = nullptr;
	uint32_t handle; // handleは初期化を遅延させる
	std::vector<Vector2> positions;
	std::vector<Vector3> Object3dpos;
	Vector3 objRotate;

	bool endRequest_ = false;

};