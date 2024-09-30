#pragma once
#include "cmath"
class Vector3 {
public:
	float x;
	float y;
	float z;

	Vector3(float x_ = 0, float y_ = 0, float z_ = 0) { x = x_, y = y_, z = z_; };

	// 加算
	Vector3 operator+(const Vector3& obj);
	// 減算
	Vector3 operator-(const Vector3& obj);
	// 乗算
	Vector3 operator*(const Vector3& obj);
	// 乗算(スカラー倍)
	Vector3 operator*(const float& scalar);
	// +=
	Vector3& operator+=(const Vector3& other);
	// 内積
	float Dot(const Vector3& v1, const Vector3& v2);
	// ベクトルの長さ
	float Length(const Vector3& v);
	// 正規化
	Vector3 Normalize(const Vector3& v);
	// 外積
	Vector3 Cross(const Vector3& v1, const Vector3& v2);
};
