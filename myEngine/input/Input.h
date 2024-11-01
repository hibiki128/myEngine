#pragma once
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include"dinput.h"
#include"windows.h"
#include "wrl.h"
#include"WinApp.h"

// 入力
class Input
{
private:
	static Input* instance;

	Input() = default;
	~Input() = default;
	Input(Input&) = delete;
	Input& operator=(Input&) = delete;


public:
	// namespace省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Input* GetInstance();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="hInstance"></param>
	/// <param name="hwnd"></param>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNuber"></param>
	/// <returns></returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber"></param>
	/// <returns></returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// キーのリリースをチェック
	/// </summary>
	/// <param name="keyNumber"></param>
	/// <returns></returns>
	bool ReleaseKey(BYTE keyNumber);

private: // メンバ変数

	// キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;
	// DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput;

	// 全キーの入力情報を取得する
	BYTE key[256] = {};
	// 前回の全キーの状態
	BYTE keyPre[256] = {};

	// WindowsAPI
	WinApp* winApp_ = nullptr;
};

