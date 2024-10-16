#pragma once
#include"DirectXCommon.h"
#include "PipeLineManager.h"
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
	std::unique_ptr<PipeLineManager> psoManager_ = nullptr;
<<<<<<< HEAD

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
=======
>>>>>>> 588d19cc7ec69ea69ec2b4e7b1eb2ed8ab6bceb3

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
	void  Initialize();

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawCommonSetting();
	
	/// <summary>
	///  getter
	/// </summary>
	/// <returns></returns>
	DirectXCommon* GetDxCommon()const { return dxCommon_; }

	/// <summary>
	/// ブレンドモードの切り替え
	/// </summary>
	void ChangeBlendMode(BlendMode blendMode);
};

