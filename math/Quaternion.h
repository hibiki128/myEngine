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

	// オイラー角からクォータニオンを生成
	static Quaternion FromEulerAngles(const Vector3& eulerAngles) {
		float pitch = eulerAngles.x * 0.5f;
		float yaw = eulerAngles.y * 0.5f;
		float roll = eulerAngles.z * 0.5f;

		float sinPitch = sinf(pitch);
		float cosPitch = cosf(pitch);
		float sinYaw = sinf(yaw);
		float cosYaw = cosf(yaw);
		float sinRoll = sinf(roll);
		float cosRoll = cosf(roll);

		return Quaternion(
			cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll, // w成分
			sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll, // x成分
			cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll, // y成分
			cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll  // z成分
		);
	}

	// クォータニオンをオイラー角に変換
	Vector3 ToEulerAngles() const;

	// クォータニオンの共役を返す
	Quaternion Conjugate() const {
		return Quaternion(w, -x, -y, -z);
	}

	// クォータニオンの正規化
	Quaternion Normalize() const {
		float length = sqrt(w * w + x * x + y * y + z * z);
		return Quaternion(w / length, x / length, y / length, z / length);
	}

	// 方向ベクトルと上ベクトルから回転クォータニオンを生成
	static Quaternion FromLookRotation(const Vector3& direction, const Vector3& up) {
		Vector3 forward = direction.Normalize();
		Vector3 right = up.Cross(forward).Normalize();
		Vector3 newUp = forward.Cross(right);

		// 回転行列の要素からクォータニオンを計算
		float w = sqrtf(1.0f + right.x + newUp.y + forward.z) * 0.5f;
		float x = (newUp.z - forward.y) / (4.0f * w);
		float y = (forward.x - right.z) / (4.0f * w);
		float z = (right.y - newUp.x) / (4.0f * w);

		return Quaternion(w, x, y, z).Normalize();
	}

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