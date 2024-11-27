#pragma once
#include"DirectXCommon.h"
#include"PipeLineManager.h"
#include <Matrix4x4.h>
class PrimitiveDrawer
{
public:
	PrimitiveDrawer();
	~PrimitiveDrawer();

	void Draw();

private:

	struct WVP {
		Matrix4x4 view;
		Matrix4x4 projection;
		Vector3 position;
	};

	struct World {
		Matrix4x4 World;
		Matrix4x4 WorldInverse;
		Matrix4x4
	};

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	DirectXCommon* dxCommon_;
	std::unique_ptr<PipeLineManager> psoManager_ = nullptr;
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

};

