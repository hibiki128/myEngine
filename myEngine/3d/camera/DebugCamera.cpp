#include "DebugCamera.h"
#include"WinApp.h"

DebugCamera::DebugCamera(int windowWidth, int windowHeight)
{
	// ビュープロジェクションの初期化
	vP_.aspectRatio = static_cast<float>(windowWidth) / windowHeight;
}

void DebugCamera::Initialize()
{
	// ビュープロジェクションの初期化
	vP_.Initialize();
	// 入力クラスのインスタンスを取得
	input_ = Input::GetInstance();
}

void DebugCamera::Update()
{
    // ホイールの回転量を取得し、zの位置を更新
    wheelData = input_->GetWheel();
    vP_.translation_.z += float(wheelData) / 120.0f; // z座標を更新

    // マウスの現在の位置を取得
    POINT cursorPos;
    GetCursorPos(&cursorPos);

    // ウィンドウ内のマウス座標に変換
    ScreenToClient(WinApp::GetInstance()->GetHwnd(), &cursorPos);

    // マウスの状態を取得
    bool isRightMouseButtonPressed = input_->PushMouseButton(1); // 右クリック
    bool isLeftMouseButtonPressed = input_->PushMouseButton(0); // 左クリック

    static POINT previousCursorPos = cursorPos; // 前回のマウス位置を保持

    // ImGuiでマウスカーソルの位置を表示
    ImGui::Begin("Cursor Position");

    // cursorPosの値をfloat配列にコピー
    float cursorPosFloat[2] = { static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y) };

    // ImGuiで値を表示
    ImGui::DragFloat2("Position", cursorPosFloat, 0.1f);

    // ImGuiでの変更をcursorPosに反映
    cursorPos.x = static_cast<LONG>(cursorPosFloat[0]);
    cursorPos.y = static_cast<LONG>(cursorPosFloat[1]);

    ImGui::End();

    // 前回のマウス位置を更新
    previousCursorPos = cursorPos;
}