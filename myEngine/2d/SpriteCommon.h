#pragma once
#include"DirectXCommon.h"
class SpriteCommon
{

private:
	static SpriteCommon* instance;

	SpriteCommon() = default;
	~SpriteCommon() = default;
	SpriteCommon(SpriteCommon&) = delete;
	SpriteCommon& operator=(SpriteCommon&) = delete;

private:
	DirectXCommon* dxCommon_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static SpriteCommon* GetInstance();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

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
	///  getter
	/// </summary>
	/// <returns></returns>
	DirectXCommon* GetDxCommon()const { return dxCommon_; }
};

