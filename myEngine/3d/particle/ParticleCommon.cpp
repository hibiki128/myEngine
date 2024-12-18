#include "ParticleCommon.h"

ParticleCommon* ParticleCommon::instance = nullptr;

ParticleCommon* ParticleCommon::GetInstance()
{
	if (instance == nullptr) {
		instance = new ParticleCommon();
	}
	return instance;
}

void ParticleCommon::Finalize()
{
	delete instance;
	instance = nullptr;
}

void ParticleCommon::Initialize(DirectXCommon* dxCommon)
{
	assert(dxCommon);
	dxCommon_ = dxCommon;
	psoManager_ = std::make_unique<PipeLineManager>();
	psoManager_->Initialize(dxCommon_);
	rootSignature = psoManager_->CreateParticleRootSignature(rootSignature);
	graphicsPipelineState = psoManager_->CreateParticleGraphicsPipeLine(graphicsPipelineState, rootSignature, blendMode_);
}

void ParticleCommon::DrawCommonSetting()
{
	psoManager_->DrawCommonSetting(graphicsPipelineState, rootSignature);
}

void ParticleCommon::SetBlendMode()
{
	psoManager_->CreateParticleGraphicsPipeLine(graphicsPipelineState, rootSignature, blendMode_);
}
