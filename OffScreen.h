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

private:
	DirectXCommon* dxCommon;
	SrvManager* srvManager_;
	std::unique_ptr<PipeLineManager> psoManager_ = nullptr;
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState[5];
	ShaderMode shaderMode_ = ShaderMode::kNone;
};

