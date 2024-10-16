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
<<<<<<< HEAD
	psoManager_->Initialize(dxCommon_);
	rootSignature = psoManager_->CreateRootSignature(rootSignature);
	graphicsPipelineState = psoManager_->CreateGraphicsPipeLine(graphicsPipelineState,rootSignature);
=======
	psoManager_->Initialize();
	psoManager_->CreateGraphicsPipeLine();
>>>>>>> 588d19cc7ec69ea69ec2b4e7b1eb2ed8ab6bceb3
}

void SpriteCommon::DrawCommonSetting()
{
<<<<<<< HEAD
	psoManager_->DrawCommonSetting(graphicsPipelineState,rootSignature);
}

void SpriteCommon::ChangeBlendMode(BlendMode blendMode)
{
	PipeLineManager::SetBlendMode(blendMode);
	Initialize();
=======
	psoManager_->DrawCommonSetting();
>>>>>>> 588d19cc7ec69ea69ec2b4e7b1eb2ed8ab6bceb3
}
