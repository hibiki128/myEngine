#pragma once
#include"DirectXCommon.h"
#include"Camera.h"
class Object3dCommon
{
private:
	static Object3dCommon* instance;

	Object3dCommon() = default;
	~Object3dCommon() = default;
	Object3dCommon(Object3dCommon&) = delete;
	Object3dCommon& operator=(Object3dCommon&) = delete;

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Object3dCommon* GetInstance();

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

