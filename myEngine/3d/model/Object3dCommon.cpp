#include "Object3dCommon.h"
#include "Logger.h"

Object3dCommon* Object3dCommon::instance = nullptr;

Object3dCommon* Object3dCommon::GetInstance()
{
	if (instance == nullptr) {
		instance = new Object3dCommon();
	}
	return instance;
}

void Object3dCommon::Finalize()
{
	delete instance;
	instance = nullptr;
}

void Object3dCommon::Initialize()
{
	// 引数で受け取ってメンバ変数に記録する
	dxCommon_ = DirectXCommon::GetInstance();
	psoManager_ = std::make_unique<PipeLineManager>();
	psoManager_->Initialize(dxCommon_);
	rootSignature = psoManager_->CreateRootSignature(rootSignature);
	graphicsPipelineState = psoManager_->CreateGraphicsPipeLine(graphicsPipelineState,rootSignature, blendMode_);

	skinningRootSignature = psoManager_->CreateSkinningRootSignature(skinningRootSignature);
	skinningGraphicsPipelineState = psoManager_->CreateSkinningGraphicsPipeLine(skinningGraphicsPipelineState, skinningRootSignature);
}

void Object3dCommon::DrawCommonSetting()
{
	psoManager_->DrawCommonSetting(graphicsPipelineState,rootSignature);
}

void Object3dCommon::skinningDrawCommonSetting()
{
	psoManager_->DrawCommonSetting(skinningGraphicsPipelineState, skinningRootSignature);
}



void Object3dCommon::SetBlendMode(BlendMode blendMode)
{
	psoManager_->CreateGraphicsPipeLine(graphicsPipelineState, rootSignature, blendMode);
}
