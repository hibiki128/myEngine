#include "SpriteCommon.h"
#include "Logger.h"

SpriteCommon* SpriteCommon::instance = nullptr;

SpriteCommon* SpriteCommon::GetInstance()
{
	if (instance == nullptr) {
		instance = new SpriteCommon();
	}
	return instance;
}

void SpriteCommon::Finalize()
{
	delete instance;
	instance = nullptr;
}

void SpriteCommon::Initialize()
{
	// 引数で受け取ってメンバ変数に記録する
	dxCommon_ = DirectXCommon::GetInstance();
	psoManager_ = std::make_unique<PipeLineManager>();
	psoManager_->Initialize(dxCommon_);
	rootSignature = psoManager_->CreateRootSignature(rootSignature);
	graphicsPipelineState = psoManager_->CreateGraphicsPipeLine(graphicsPipelineState,rootSignature,blendMode_);
}

void SpriteCommon::DrawCommonSetting()
{
	psoManager_->DrawCommonSetting(graphicsPipelineState,rootSignature);
}

void SpriteCommon::SetBlendMode(BlendMode blendMode)
{
	psoManager_->CreateGraphicsPipeLine(graphicsPipelineState, rootSignature, blendMode);
}
