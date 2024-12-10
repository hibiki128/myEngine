#include "FollowCamera.h"
#include "Input.h"

void FollowCamera::Init() {
	viewProjection_.farZ = 1100;
	viewProjection_.Initialize();
}

void FollowCamera::Update() {
    // 追従対象がいれば
    if (target_) {
        // 追従対象からカメラまでのオフセット
        Vector3 offset = { 0.0f, 7.0f, -25.0f };

        XINPUT_STATE joyState;
        bool isJoystickConnected = Input::GetInstance()->GetJoystickState(0, joyState);

        if (isJoystickConnected) {
            // ゲームパッドが接続されている場合の処理
            const float kRotateSpeed = 0.1f;

            Vector3 move;
            move.x = 0;
            move.y += static_cast<float>(joyState.Gamepad.sThumbRX);
            move.z = 0;

            // 移動量に速さを反映
            if (move.Length() > 0.0f) {
                move = move.Normalize() * kRotateSpeed;
            }
            else {
                move = { 0.0f, 0.0f, 0.0f };
            }

            viewProjection_.rotation_ += move;
        }
        else {
            // ゲームパッドが接続されていない場合
            // targetの位置にオフセットを適用してカメラの位置を更新
            viewProjection_.translation_ = target_->translation_ + offset;
        }

        Matrix4x4 rotateMatrix = MakeRotateXYZMatrix(viewProjection_.rotation_);

        // オフセットをカメラの回転に合わせて回転させる
        offset = TransformNormal(offset, rotateMatrix);

        // 座標をコピーしてオフセット分ずらす
        viewProjection_.translation_ = target_->translation_ + offset;
    }

    // ビュー行列の更新
    viewProjection_.UpdateMatrix();
}
