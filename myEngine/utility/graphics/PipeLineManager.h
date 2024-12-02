#pragma once
#include "wrl.h"
#include "d3d12.h"
#include <DirectXCommon.h>

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
	kMultiply,
	// スクリーン
	kScreen,
};

enum class ShaderMode {
	kNone,
	kGray
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
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRenderRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateRenderGraphicsPipeLine(Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature, ShaderMode shaderMode_);

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawCommonSetting(Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatur);

private:
	DirectXCommon* dxCommon_;
};
