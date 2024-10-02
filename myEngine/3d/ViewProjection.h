#pragma once
#include"Matrix4x4.h"
#include"Vector3.h"
#include "wrl.h"
#include"numbers"
#include"myMath.h"
#include "d3d12.h"
#include"DirectXCommon.h"

struct ConstBufferDataViewProjection {
	Matrix4x4 view;
	Matrix4x4 projection;
	Vector3 cameraPos;
};

class ViewProjection
{
public:
	// ローカル回転角
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };
	// ローカル座標
	Vector3 translation_ = { 0.0f,0.0f,-50.0f };

	// 垂直方向視野角
	float fovAngleY = 45.0f * std::numbers::pi_v<float>;
	// ビューポートのアスペクト比
	float aspectRatio = (float)16 / 9;
	// 深度限界(手前側)
	float nearZ = 0.1f;
	// 深度限界(奥側)
	float farZ = 1000.0f;

	// ビュー行列
	Matrix4x4 matView_;
	// 射影行列
	Matrix4x4 matProjection_;

	ViewProjection() = default;
	~ViewProjection() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();

	/// <summary>
	/// 行列を更新する
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// 行列を転送する
	/// </summary>
	void TransferMatrix();

	/// <summary>
	/// ビュー行列を更新する
	/// </summary>
	void UpdateViewMatrix();

	/// <summary>
	/// 射影行列を更新する
	/// </summary>
	void UpdateProjectionMatrix();

	/// <summary>
	/// 定数バッファの取得
	/// </summary>
	/// <returns>定数バッファ</returns>
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }
private:

	DirectXCommon* dxCommon_ = nullptr;

	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
	// マッピング済みアドレス
	ConstBufferDataViewProjection* constMap = nullptr;
	// コピー禁止
	ViewProjection(const ViewProjection&) = delete;
	ViewProjection& operator=(const ViewProjection&) = delete;

};

static_assert(!std::is_copy_assignable_v<ViewProjection>);