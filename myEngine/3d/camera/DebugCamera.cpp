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
    const float mouseSensitivity = 0.003f;
    const float moveSpeed = 0.005f;
    static bool isLeftClicked = false;
    static bool isWheelClicked = false;

    Matrix4x4 rotationMatrix = MakeRotateXYZMatrix(vP_.rotation_);
    Vector3 X = { 1.0f,0.0f,0.0f };
    Vector3 Y = { 0.0f,1.0f,0.0f };
    Vector3 Z = { 0.0f,0.0f,-1.0f };

    Vector3 rotatedX = Transformation(X, rotationMatrix);
    Vector3 rotatedY = Transformation(Y, rotationMatrix);
    Vector3 rotatedZ = Transformation(Z, rotationMatrix);
    Vector2 clickPosition{};

    if (input_->PushMouseButton(0)) {
        if (!isLeftClicked) {
            clickPosition = input_->GetMousePos();
            isLeftClicked = true;
        }
        Vector2 currentMousePos = input_->GetMousePos();

        float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
        float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

        vP_.rotation_.x = deltaY * mouseSensitivity;
        vP_.rotation_.y = deltaX * mouseSensitivity;

        clickPosition = currentMousePos;
    }
    else {
        isLeftClicked = false;
    }

    if (input_->PushMouseButton(2)) {
        if (!isWheelClicked) {
            clickPosition = input_->GetMousePos();
            isWheelClicked = true;
        }
        else {
            Vector2 currentMousePos;
            currentMousePos = input_->GetMousePos();

            float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
            float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

            vP_.translation_ = rotatedX * deltaX * mouseSensitivity;
            vP_.translation_ = rotatedY * deltaY * mouseSensitivity;

            clickPosition = currentMousePos;
        }
    }
    else {
        isWheelClicked = false;
    }

    // ホイールの回転量を取得し、zの位置を更新
    wheelData = input_->GetWheel();
    vP_.translation_.z = float(wheelData) / 120.0f; // z座標を更新
    vP_.TransferMatrix();
}