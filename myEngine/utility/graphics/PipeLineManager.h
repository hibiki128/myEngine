#pragma once
#include "wrl.h"
#include "d3d12.h"
#include <DirectXCommon.h>

// static を削除
enum class BlendMode {
	// ブレンドなし
	kNone,
	// 通常ブレンド
	kNormal,
	// 加算
	kAdd,
	// 減算
	kSubtract,
	// 乗算
	kMultiply, // スペルミスを修正
	// スクリーン
	kScreen,
};

class PipeLineManager
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipeLine(Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature, BlendMode blendMode_);

	/// <summary>
	/// パーティクル用のルートシグネチャの作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateParticleRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

	/// <summary>
	/// パーティクル用パイプラインの作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateParticleGraphicsPipeLine(Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature, BlendMode blendMode_);

	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateSpriteRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateSpriteGraphicsPipeLine(Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature, BlendMode blendMode_);

	/// <summary>
    /// ルートシグネチャの作成
    /// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateLine3dRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateLine3dGraphicsPipeLine(Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawCommonSetting(Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);
private:
	DirectXCommon* dxCommon_;
};
