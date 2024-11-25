#pragma once
#include <d3d12.h>
#include"wrl.h"
#include <PipeLineManager.h>
#include"SrvManager.h"
class DirectXCommon;
class OffScreen
{
public:
	void Initialize(const std::string& textureFilePath);

	void Draw();

private:
	void CreateVartexResource();
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexGrayResource_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	DirectXCommon* dxCommon;
	SrvManager* srvManager_;
	std::unique_ptr<PipeLineManager> psoManager_ = nullptr;
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	BlendMode blendMode_ = BlendMode::kNormal;

	std::string directoryPath_ = "resources/images";
	std::string fullpath;
};

