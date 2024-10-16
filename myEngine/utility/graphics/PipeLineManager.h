#pragma once
<<<<<<< HEAD
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
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipeLine(Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);
    
    /// <summary>
    /// パーティクル用のルートシグネチャの作成
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateParticleRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

    /// <summary>
    /// パーティクル用パイプラインの作成
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateParticleGraphicsPipeLine(Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

    /// <summary>
   /// Primitive用のルートシグネチャの作成
   /// </summary>
    Microsoft::WRL::ComPtr<ID3D12RootSignature> CreatePrimitiveRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

    /// <summary>
    /// Primitive用パイプラインの作成
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreatePrimitiveGraphicsPipeLine(Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

    /// <summary>
    /// 共通描画設定
    /// </summary>
    void DrawCommonSetting(Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

    /// <summary>
    /// セッター (静的メンバ関数)
    /// </summary>
    /// <param name="blendMode"></param>
    static void SetBlendMode(BlendMode blendMode) {
        blendMode_ = blendMode;
    }

private:
    DirectXCommon* dxCommon_;

    // 静的なブレンドモード変数
    static BlendMode blendMode_;
};
=======
#include"wrl.h"
#include "d3d12.h"
#include <DirectXCommon.h>

class PipeLineManager
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

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
private:
	DirectXCommon* dxCommon_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
};

>>>>>>> 588d19cc7ec69ea69ec2b4e7b1eb2ed8ab6bceb3
