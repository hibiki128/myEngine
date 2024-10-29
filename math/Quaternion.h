#pragma once
#include "Vector3.h"
#include <cmath>

class Quaternion {
public:
    float w, x, y, z;

    // コンストラクタ
    Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

    // 2つのベクトルの間の回転を計算
    void SetFromTo(const Vector3& from, const Vector3& to);

    // クォータニオンをオイラー角に変換
    Vector3 ToEulerAngles() const;

    // クォータニオン同士の掛け算をオーバーロード
    Quaternion operator*(const Quaternion& q) const {
        return Quaternion(
            w * q.w - x * q.x - y * q.y - z * q.z,                      // スカラー成分
            w * q.x + x * q.w + y * q.z - z * q.y,                      // x成分
            w * q.y - x * q.z + y * q.w + z * q.x,                      // y成分
            w * q.z + x * q.y - y * q.x + z * q.w                       // z成分
        );
    }
};