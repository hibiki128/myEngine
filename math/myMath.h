#pragma once
#include"Matrix4x4.h"
#include "Vector4.h"
#include "assert.h"
#include "cmath"
#include"vector"
#include"algorithm"
#include <Vector3.h>


static const int kColumnWidth = 60;
static const int kRowHeight = 20;

float Lerp(float _start, float _end, float _t);
Vector3 Lerp(const Vector3& _start, const Vector3& _end, float _t);
Vector4 Lerp(const Vector4& _start, const Vector4& _end, float _t);

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// 座標変換
Vector3 Transformation(const Vector3& vector, const Matrix4x4& matrix);
Vector4 Transformation(const Vector4& vector, const Matrix4x4& matrix);

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

Vector3 TransformVector(const Vector3& vector, const Matrix4x4& matrix);

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
Matrix4x4 MakeRotateXYZMatrix(const Vector3& radian);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// tanθの逆数
float cotf(float theta);

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// ビューポート変換行列
Matrix4x4 MakeViewPortMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);

Matrix4x4 CreateRotationMatrix(const Vector3& eulerAngles);

//// デバッグ用
//void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
//void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);
