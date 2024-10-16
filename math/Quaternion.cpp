#include "Quaternion.h"
#include"numbers"

void Quaternion::SetFromTo(const Vector3& from, const Vector3& to)
{
    Vector3 f = from.Normalize(); // 正規化したfromベクトル
    Vector3 t = to.Normalize();   // 正規化したtoベクトル

    Vector3 cross = f.Cross(t);    // fromとtoのクロス積
    float dot = f.Dot(t);          // fromとtoの内積

    // 回転角をクォータニオンに変換
    w = sqrt((1.0f + dot) * 0.5f); // 回転のスカラー成分
    float s = 0.5f / w;

    x = cross.x * s;
    y = cross.y * s;
    z = cross.z * s;
}

Vector3 Quaternion::ToEulerAngles() const
{
    Vector3 angles;

    // ピッチ（X軸）
    float sinPitch = 2.0f * (w * x + y * z);
    float cosPitch = 1.0f - 2.0f * (x * x + y * y);
    angles.x = atan2(sinPitch, cosPitch);

    // ヨー（Y軸）
    float sinYaw = 2.0f * (w * y - z * x);
    angles.y = fabs(sinYaw) >= 1.0f ? copysign(std::numbers::pi_v<float> / 2, sinYaw) : asin(sinYaw); // 特別なケース

    // ロール（Z軸）
    float sinRoll = 2.0f * (w * z + x * y);
    float cosRoll = 1.0f - 2.0f * (y * y + z * z);
    angles.z = atan2(sinRoll, cosRoll);

    return angles;
}
