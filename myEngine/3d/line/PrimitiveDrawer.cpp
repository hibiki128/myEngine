#include "PrimitiveDrawer.h"

PrimitiveDrawer::PrimitiveDrawer()
{
	dxCommon_ = DirectXCommon::GetInstance();
	psoManager_ = std::make_unique<PipeLineManager>();
	psoManager_->Initialize(dxCommon_);
	rootSignature = psoManager_->CreateParticleRootSignature(rootSignature);
	graphicsPipelineState = psoManager_->CreatePrimitiveGraphicsPipeLine(graphicsPipelineState, rootSignature);
}

PrimitiveDrawer::~PrimitiveDrawer()
{
}

void PrimitiveDrawer::Draw()
{
}
