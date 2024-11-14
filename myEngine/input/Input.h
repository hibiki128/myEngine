#pragma once
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include "dinput.h"
#include "windows.h"
#include "wrl.h"
#include "WinApp.h"
#include"Vector2.h"

// 入力
class Input
{
private:
    static Input* instance;

    Input() : wheelState(0) {} // 初期化時にホイールの状態を0に設定
    ~Input() = default;
    Input(Input&) = delete;
    Input& operator=(Input&) = delete;

public:
    // namespace省略
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

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
    /// <param name="keyNumber"></param>
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

    /// <summary>
    /// マウスボタンの押下をチェック
    /// </summary>
    /// <param name="buttonNumber">0: 左クリック, 1: 右クリック, 2: ホイールクリック</param>
    /// <returns></returns>
    bool PushMouseButton(int buttonNumber);

    /// <summary>
    /// マウスボタンのトリガーをチェック
    /// </summary>
    /// <param name="buttonNumber">0: 左クリック, 1: 右クリック, 2: ホイールクリック</param>
    /// <returns></returns>
    bool TriggerMouseButton(int buttonNumber);

    /// <summary>
    /// マウスのX座標取得
    /// </summary>
    /// <returns>X軸の移動量</returns>
    long GetMouseX() const { return mouseState.lX; }

    /// <summary>
    /// マウスのY座標取得
    /// </summary>
    /// <returns>Y軸の移動量</returns>
    long GetMouseY() const { return mouseState.lY; }

    /// <summary>
    /// マウスホイールの回転量取得
    /// </summary>
    /// <returns>ホイールの回転量</returns>
    int GetWheel() const { return wheelState; }

    Vector2 GetMousePos();

private: // メンバ変数
    // キーボードのデバイス
    ComPtr<IDirectInputDevice8> keyboard;
    // マウスのデバイス
    ComPtr<IDirectInputDevice8> mouse;
    // DirectInputのインスタンス
    ComPtr<IDirectInput8> directInput;

    // 全キーの入力情報を取得する
    BYTE key[256] = {};
    // 前回の全キーの状態
    BYTE keyPre[256] = {};

    // マウスボタンの状態
    DIMOUSESTATE2 mouseState = {};
    DIMOUSESTATE2 mouseStatePre = {};

    // ホイールの回転状態
    int wheelState; // ホイールの状態

    // WindowsAPI
    WinApp* winApp_ = nullptr;
};
