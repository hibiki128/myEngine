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
	graphicsPipelineState = psoManager_->CreateGraphicsPipeLine(graphicsPipelineState,rootSignature);
}

void Object3dCommon::DrawCommonSetting()
{
	psoManager_->DrawCommonSetting(graphicsPipelineState,rootSignature);
}

void Object3dCommon::ChangeBlendMode(BlendMode blendMode)
{
	PipeLineManager::SetBlendMode(blendMode);
	Initialize();
}
