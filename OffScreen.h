#pragma once
#include <d3d12.h>
#include"wrl.h"
#include <PipeLineManager.h>
#include"SrvManager.h"
class DirectXCommon;
class OffScreen
{
public:
	void Initialize();

	void Draw();

	void DrawCommonSetting();
private:

	void CreateSmooth();
	void CreateGauss();
	void CreateVignette();

private:
	DirectXCommon* dxCommon;
	SrvManager* srvManager_;
	std::unique_ptr<PipeLineManager> psoManager_ = nullptr;
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature[4];

	// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState[6];
	ShaderMode shaderMode_ = ShaderMode::kNone;


	struct KernelSettings {
		int kernelSize;
	};

	struct GaussianParams {
		int kernelSize;
		float sigma;
	};

	struct VignetteParameter {
		float vignetteStrength;
		float vignetteRadius;
		float vignetteExponent;
	};
	
	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vignetteResource;
	// バッファリソース内のデータを指すポインタ
	VignetteParameter* vignetteData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> smoothResource;
	// バッファリソース内のデータを指すポインタ
	KernelSettings* smoothData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> gaussianResouce;
	// バッファリソース内のデータを指すポインタ
	GaussianParams* gaussianData = nullptr;
};

