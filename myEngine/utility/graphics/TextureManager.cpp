#include "TextureManager.h"
#include "DirectXCommon.h"
#include "StringUtility.h"

TextureManager* TextureManager::instance = nullptr;

// ImGuiで0番を使用するため、1番から使用
uint32_t TextureManager::kSRVIndexTop = 1;

void TextureManager::LoadTexture(const std::string& filePath)
{
	// 読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		return;
	}

	// テクスチャ枚数上限をチェック
	assert(srvManager_->CanAllocate());

	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// テクスチャデータを追加して書き込む
	TextureData& textureData = textureDatas[filePath];

    textureData.metadata = image.GetMetadata();
    textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);
	textureData.intermediateResource = dxCommon_->UploadTextureData(textureData.resource, image);

	textureData.srvIndex = srvManager_->Allocate()+kSRVIndexTop;
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	srvManager_->CreateSRVforTexture2D(textureData.srvIndex, textureData.resource.Get(), textureData.metadata.format, UINT(textureData.metadata.mipLevels));

}

void TextureManager::Initialize(SrvManager* srvManager)
{
	dxCommon_ = DirectXCommon::GetInstance();
	srvManager_ = srvManager;
	// SRVの数と同数
	textureDatas.reserve(SrvManager::kMaxSRVCount);
}

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	// unordered_mapを使って直接インデックスを取得
	auto it = textureDatas.find(filePath);
	if (it != textureDatas.end()) {
		return it->second.srvIndex;
	}

	// 見つからない場合はassertでエラーにする
	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{
	// 指定されたファイルパスが存在するかチェック
	assert(textureDatas.find(filePath) != textureDatas.end());

	TextureData& textureData = textureDatas[filePath];
	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	// 指定されたファイルパスが存在するかチェック
	assert(textureDatas.find(filePath) != textureDatas.end());

	TextureData& textureData = textureDatas[filePath];
	return textureData.metadata;
}
