#pragma once
#include"DirectXCommon.h"
#include"Camera.h"
class Object3dCommon
{
public: // メンバ関数
	/// <summary>
    ///  初期化
    /// </summary>
	void  Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	void CreateGraphicsPipeLine();

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawCommonSetting();

	/// <summary>
	/// setter
	/// </summary>
	/// <param name="camera"></param>
	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }

	/// <summary>
	///  getter
	/// </summary>
	/// <returns></returns>
	DirectXCommon* GetDxCommon()const { return dxCommon_; }
	Camera* GetDefaultCamera()const { return defaultCamera; }

private:
	DirectXCommon* dxCommon_;
	Camera* defaultCamera = nullptr;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
};

