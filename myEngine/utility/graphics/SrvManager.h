#pragma once
#include "cstdint"
#include"wrl.h"
#include"d3d12.h"
class DirectXCommon;
class SrvManager
{
private:
	DirectXCommon* dxCommon = nullptr;

	// SRV用のでスクリプタサイズ
	uint32_t descriptorSize;
	// SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	// 次に使用するSRVインデックス
	uint32_t useIndex = 0;

public:
	// 最大SRV数(最大テクスチャ枚数)
	static const uint32_t kMaxSRVCount;
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// SRV生成(テクスチャ用)
	/// </summary>
	/// <param name="srvIndex"></param>
	/// <param name="pResource"></param>
	/// <param name="Format"></param>
	/// <param name="MipLevels"></param>
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

	/// <summary>
	/// SRV生成(Structured Buffer用)
	/// </summary>
	/// <param name="srvIndex"></param>
	/// <param name="pResource"></param>
	/// <param name="numElements"></param>
	/// <param name="structureByteStride"></param>
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	/// <summary>
	/// インデックス割り当て
	/// </summary>
	/// <returns></returns>
	uint32_t Allocate();

	bool CanAllocate() const;

	/// <summary>
	/// getter
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// setter
	/// </summary>
	/// <param name="RootParameterIndex"></param>
	/// <param name="srvIndex"></param>
	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);


};

