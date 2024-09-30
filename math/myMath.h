#pragma once
#include"Matrix4x4.h"
#include "Vector3.h"
#include "assert.h"
#include "cmath"


static const int kColumnWidth = 60;
static const int kRowHeight = 20;

float Dot(const Vector3& v1, const Vector3& v2);

float LengthSquared(const Vector3& v);

float Length(const Vector3& v);

Vector3 Cross(const Vector3& v1, const Vector3& v2);

Vector3 Normalize(const Vector3& v);

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// 座標変換
Vector3 Transformation(const Vector3& vector, const Matrix4x4& matrix);

// 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

// 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m);

// 単位行列の作成
Matrix4x4 MakeIdentity4x4();

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);
// Y軸回転行列							
Matrix4x4 MakeRotateYMatrix(float radian);
// Z軸回転行列							
Matrix4x4 MakeRotateZMatrix(float radian);
// X,Y,Z軸回転行列を合成した行列
Matrix4x4 MakeRotateXYZMatrix(Vector3& radian);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// tanθの逆数
float cotf(float theta);

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// ビューポート変換行列
Matrix4x4 MakeViewPortMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

//// デバッグ用
//void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
//void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);
