#pragma once
#include "ViewProjection.h"
#include "Input.h"
#include "Matrix4x4.h"
#include <Vector2.h>
#include <Windows.h>  // GetCursorPosを使用するために追加

class DebugCamera
{
public:
    DebugCamera(int windowWidth, int windowHeight);

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    void SetFarZ(float farZ) { vP_.farZ = farZ; }

    /// <summary>
    /// ビュープロジェクションを取得
    /// </summary>
    /// <returns>ビュープロジェクション</returns>
    const ViewProjection& GetViewProjection() { return vP_; }

private:
    // ビュープロジェクション行列
    ViewProjection vP_;
    // 入力クラスのポインタ
    Input* input_;
    // 回転行列
    Matrix4x4 matRot_;

    int wheelData = 0;
    float rotateX = 0.0f;       // X軸の回転角度
    float rotateY = 0.0f;       // Y軸の回転角度
    float sensitivity = 0.1f;    // マウスの動きに対する感度
};