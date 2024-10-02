#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

Input* Input::instance = nullptr;

Input* Input::GetInstance()
{
	if (instance == nullptr) {
		instance = new Input;
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
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

}

void Input::Update()
{
	HRESULT result;

	// 前回のキー入力を保存
	memcpy(keyPre, key, sizeof(key));

	// キーボード情報の取得開始
	result = keyboard->Acquire();

	result = keyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE keyNumber)
{
	// 指定キーを押していればtrueを返す
	if (key[keyNumber]) {
		return true;
	}
	// そうでなければfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	// 指定キーを押した瞬間trueを返す
	if (!keyPre[keyNumber] && key[keyNumber]) {
		return true;
	}
	// そうでなければfalseを返す
	return false;
}
