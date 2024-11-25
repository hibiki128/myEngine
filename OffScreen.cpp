#include "OffScreen.h"
#include"DirectXCommon.h"
#include <TextureManager.h>

void OffScreen::Initialize(const std::string& textureFilePath)
{
	fullpath = directoryPath_ + "/" + textureFilePath;
	dxCommon = DirectXCommon::GetInstance();
	psoManager_ = std::make_unique<PipeLineManager>();
	psoManager_->Initialize(dxCommon);
	rootSignature = psoManager_->CreateRenderRootSignature(rootSignature);
	graphicsPipelineState = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState, rootSignature, blendMode_);
}

void OffScreen::Draw()
{
	psoManager_->DrawCommonSetting(graphicsPipelineState, rootSignature);
	srvManager_ = TextureManager::GetInstance()->GetSrvManager();
	srvManager_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureIndexByFilePath(fullpath));
	dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void OffScreen::CreateVartexResource()
{

}
