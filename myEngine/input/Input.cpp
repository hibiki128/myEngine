#include "Input.h"
#include<assert.h>
#include"myMath.h"
#include <cmath>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

std::unique_ptr<Mouse>Input::mouse_ = nullptr;

template bool Input::GetJoystickState<DIJOYSTATE2>(int32_t stickNo, DIJOYSTATE2& out) const;
template bool Input::GetJoystickState<XINPUT_STATE>(int32_t stickNo, XINPUT_STATE& out) const;
template bool Input::GetJoystickStatePrevious<DIJOYSTATE2>(int32_t stickNo, DIJOYSTATE2& out) const;
template bool Input::GetJoystickStatePrevious<XINPUT_STATE>(int32_t stickNo, XINPUT_STATE& out) const;

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Init(HINSTANCE hInstance, HWND hWnd) {
	//DirectInputの初期化
	HRESULT result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(
		hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	//マウス初期化
	mouse_ = std::make_unique<Mouse>();
	mouse_->Init(directInput_, hWnd);

	//XInputデバイスの追加
	for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		if (XInputGetState(i, &state) == ERROR_SUCCESS) {
			Joystick joystick = {};
			joystick.type_ = PadType::XInput;
			joystick.state_ = state;
			joystick.statePre_ = state;
			joystick.deadZoneL_ = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			joystick.deadZoneR_ = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			joysticks_.push_back(joystick);
		}
	}
}

void Input::Update() {
	keyPre_ = key_;
	//キーボードの情報の取得開始
	keyboard_->Acquire();
	//全キーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(key_), key_.data());

	mouse_->Update();//マウス更新


	// マウスの位置を更新
	/*mousePosition_.x += static_cast<float>(mouse_.lX);
	mousePosition_.y += static_cast<float>(mouse_.lY);*/

	for (auto& joystick : joysticks_) {
		if (joystick.type_ == PadType::XInput) {
			DWORD dwResult;
			XINPUT_STATE state;
			ZeroMemory(&state, sizeof(XINPUT_STATE));
			dwResult = XInputGetState(0, &state);
			if (dwResult == ERROR_SUCCESS) {
				joystick.statePre_ = joystick.state_;
				joystick.state_ = state;
			}
		}
	}
}

//キーボード*************************************************************
bool Input::PushKey(BYTE keyNumber)const {
	return (key_[keyNumber] & 0x80);
}


bool Input::TriggerKey(BYTE keyNumber)const {
	return (key_[keyNumber] & 0x80) && !(keyPre_[keyNumber] & 0x80);
}


bool Input::ReleaseKey(BYTE keyNumber)const {
	return !(key_[keyNumber] & 0x80);
}


bool Input::ReleaseMomentKey(BYTE keyNumber)const {
	return !(key_[keyNumber] & 0x80) && (keyPre_[keyNumber] & 0x80);
}

//ゲームパッド*******************************************************************

template<typename T>bool Input::GetJoystickState(int32_t stickNo, T& out)const {
	if (stickNo < 0 || stickNo >= joysticks_.size()) {
		return false;
	}
	const Joystick& joystick = joysticks_[stickNo];
	if (joystick.type_ == PadType::DirectInput) {
		if constexpr (std::is_same<T, DIJOYSTATE2>::value) {
			if (std::holds_alternative<T>(joystick.state_)) {
				out = std::get<T>(joystick.state_);
				return true;
			}
		}
	}
	else if (joystick.type_ == PadType::XInput) {
		if constexpr (std::is_same<T, XINPUT_STATE>::value) {
			if (std::holds_alternative<T>(joystick.state_)) {
				out = std::get<T>(joystick.state_);
				return true;
			}
		}
	}
	return false;
}


template<typename T>bool Input::GetJoystickStatePrevious(int32_t stickNo, T& out) const {
	if (stickNo < 0 || stickNo >= joysticks_.size()) {
		return false;
	}
	const Joystick& joystick = joysticks_[stickNo];
	if (joystick.type_ == PadType::DirectInput) {
		if constexpr (std::is_same<T, DIJOYSTATE2>::value) {
			if (std::holds_alternative<T>(joystick.statePre_)) {
				out = std::get<T>(joystick.statePre_);
				return true;
			}
		}
	}
	else if (joystick.type_ == PadType::XInput) {
		if constexpr (std::is_same<T, XINPUT_STATE>::value) {
			if (std::holds_alternative<T>(joystick.statePre_)) {
				out = std::get<T>(joystick.statePre_);
				return true;
			}
		}
	}
	return false;
}

void Input::SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR) {

	if (stickNo < 0 || stickNo >= static_cast<int32_t>(joysticks_.size())) {
		return;
	}

	joysticks_[stickNo].deadZoneL_ = deadZoneL;
	joysticks_[stickNo].deadZoneR_ = deadZoneR;
}

size_t Input::GetNumberOfJoysticks()const {
	return joysticks_.size();
}

//マウス****************************************************************

bool Input::IsPressMouse(int32_t buttonNumber) {
	return mouse_->IsPressMouse(buttonNumber);
}

bool Input::IsTriggerMouse(int32_t buttonNumber) {
	return mouse_->IsTriggerMouse(buttonNumber);
}

MouseMove Input::GetMouseMove() {
	return mouse_->GetMouseMove();
}

Vector3 Input::GetMousePos3D(const ViewProjection& viewprojection, float depthFactor, float blockSpacing) {
	return mouse_->GetMousePos3D(viewprojection, depthFactor, blockSpacing);
}


int32_t Input::GetWheel() {
	return mouse_->GetWheel();
}

Vector2 Input::GetMousePos() {
	return mouse_->GetMousePos();
}

void Mouse::Init(Microsoft::WRL::ComPtr<IDirectInput8>directInput, HWND hWnd) {
	hWnd_ = hWnd;
	//マウスデバイスの生成
	HRESULT  result = directInput->CreateDevice(GUID_SysMouse, &devMouse_, NULL);
	assert(SUCCEEDED(result));
	mousePosition_ = { 0.0f, 0.0f }; // 初期値の確認

	//入力データ形式のセット
	result = devMouse_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = devMouse_->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
}

void Mouse::Update() {
	// マウスの状態を更新
	mousePre_ = mouse_;
	devMouse_->Acquire();
	devMouse_->GetDeviceState(sizeof(mouse_), &mouse_);
}

//マウス****************************************************************

bool Mouse::IsPressMouse(int32_t buttonNumber)const {
	return(mouse_.rgbButtons[buttonNumber] & 0x80);
}

bool Mouse::IsTriggerMouse(int32_t buttonNumber)const {
	return (mouse_.rgbButtons[buttonNumber] & 0x80) && !(mousePre_.rgbButtons[buttonNumber] & 0x80);
}

MouseMove Mouse::GetMouseMove() {
	MouseMove move;
	move.lX = mouse_.lX;
	move.lY = mouse_.lY;
	move.lZ = mouse_.lZ;
	return move;
}

Vector3 Mouse::GetMousePos3D(const ViewProjection& viewprojection, float depthFactor, float blockSize) const {
	// 2Dマウス座標を取得
	Vector2 mousePos = mousePosition_;

	// ウィンドウサイズ
	float windowWidth = 1280.0f;
	float windowHeight = 720.0f;

	// スクリーン座標を正規化デバイス座標 (NDC) に変換 [-1, 1] の範囲にする
	float ndcX = (2.0f * mousePos.x / windowWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * mousePos.y / windowHeight);
	float ndcZ = depthFactor; // Z軸の奥行きを調整するパラメータ

	// NDC座標をVector3に変換（NDCのZ値をdepthFactorで調整）
	Vector3 clipPos = { ndcX, ndcY, ndcZ };

	// 逆射影行列を使ってクリップ空間からビュー空間へ変換
	Matrix4x4 invProj = Inverse(viewprojection.matProjection_);
	Vector3 viewPos = Transformation(clipPos, invProj);

	// ビュー空間からワールド空間へ変換
	Matrix4x4 invView = Inverse(viewprojection.matView_);
	Vector3 worldPos = Transformation(viewPos, invView);

	// ブロックサイズに基づいてスナップ処理を適用
	if (blockSize > 0.0f) {
		worldPos.x = std::round(worldPos.x / blockSize) * blockSize;
		worldPos.y = std::round(worldPos.y / blockSize) * blockSize;
		worldPos.z = std::round(worldPos.z / blockSize) * blockSize;
	}

	// ワールド座標を返す
	return worldPos;
}

int32_t Mouse::GetWheel() const {
	return mouse_.lZ;
}

Vector2 Mouse::GetMousePos() {
	// マウス座標を取得
	POINT mousePos;
	GetCursorPos(&mousePos);
	// スクリーン座標からウィンドウ内座標に変換
	ScreenToClient(hWnd_, &mousePos); // hWndはウィンドウハンドル
	mousePosition_ = Vector2(float(mousePos.x), float(mousePos.y));

	return mousePosition_;
}