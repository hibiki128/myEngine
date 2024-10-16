#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

Input* Input::instance = nullptr;

Input* Input::GetInstance()
{
    if (instance == nullptr) {
        instance = new Input();
    }
    return instance;
}

void Input::Finalize()
{
    delete instance;
    instance = nullptr;
}

void Input::Initialize(WinApp* winApp)
{
    HRESULT result;

    this->winApp_ = winApp;

    result = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
    assert(SUCCEEDED(result));

    // キーボードデバイスの初期化
    result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
    assert(SUCCEEDED(result));
    result = keyboard->SetDataFormat(&c_dfDIKeyboard);
    assert(SUCCEEDED(result));
    result = keyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
    assert(SUCCEEDED(result));

    // マウスデバイスの初期化
    result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
    assert(SUCCEEDED(result));
    result = mouse->SetDataFormat(&c_dfDIMouse2);
    assert(SUCCEEDED(result));
    result = mouse->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    assert(SUCCEEDED(result));
}

void Input::Update()
{
    HRESULT result;

    // 前回のキー入力を保存
    memcpy(keyPre, key, sizeof(key));
    // 前回のマウス入力を保存
    mouseStatePre = mouseState;

    // キーボード情報の取得開始
    result = keyboard->Acquire();
    result = keyboard->GetDeviceState(sizeof(key), key);

    // マウス情報の取得開始
    result = mouse->Acquire();
    result = mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState);

    // ホイールの回転量をリセット
    if (mouseState.lZ != 0) {
        wheelState += mouseState.lZ; // 現在の回転量を加算
    }
}

bool Input::PushKey(BYTE keyNumber)
{
    // 指定キーを押していればtrueを返す
    return key[keyNumber] != 0;
}

bool Input::TriggerKey(BYTE keyNumber)
{
    // 指定キーを押した瞬間trueを返す
    return !keyPre[keyNumber] && key[keyNumber];
}

bool Input::ReleaseKey(BYTE keyNumber)
{
    // 指定キーを離した瞬間trueを返す
    return keyPre[keyNumber] && !key[keyNumber];
}

bool Input::PushMouseButton(int buttonNumber)
{
    // 指定マウスボタンを押している場合trueを返す
    return (mouseState.rgbButtons[buttonNumber] & 0x80) != 0;
}

bool Input::TriggerMouseButton(int buttonNumber)
{
    // 指定マウスボタンを押した瞬間trueを返す
    return !(mouseStatePre.rgbButtons[buttonNumber] & 0x80) && (mouseState.rgbButtons[buttonNumber] & 0x80);
}