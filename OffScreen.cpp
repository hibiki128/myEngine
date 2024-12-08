#include "OffScreen.h"
#include"DirectXCommon.h"
#include"imgui.h"


void OffScreen::Initialize()
{
	dxCommon = DirectXCommon::GetInstance();
	psoManager_ = std::make_unique<PipeLineManager>();
	psoManager_->Initialize(dxCommon);
	rootSignature[0] = psoManager_->CreateRenderRootSignature(rootSignature[0], ShaderMode::kNone);
	rootSignature[1] = psoManager_->CreateRenderRootSignature(rootSignature[1], ShaderMode::kVigneet);
	rootSignature[2] = psoManager_->CreateRenderRootSignature(rootSignature[2], ShaderMode::kSmooth);
	rootSignature[3] = psoManager_->CreateRenderRootSignature(rootSignature[3], ShaderMode::kGauss);
	rootSignature[4] = psoManager_->CreateRenderRootSignature(rootSignature[4], ShaderMode::kDepth);
	graphicsPipelineState[0] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[0], rootSignature[0], ShaderMode::kNone);
	graphicsPipelineState[1] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[1], rootSignature[0], ShaderMode::kGray);
	graphicsPipelineState[2] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[2], rootSignature[1], ShaderMode::kVigneet);
	graphicsPipelineState[3] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[3], rootSignature[2], ShaderMode::kSmooth);
	graphicsPipelineState[4] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[4], rootSignature[3], ShaderMode::kGauss);
	graphicsPipelineState[5] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[5], rootSignature[0], ShaderMode::kOutLine);
	graphicsPipelineState[6] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[5], rootSignature[4], ShaderMode::kDepth);

	CreateSmooth();
	CreateGauss();
	CreateVignette();
	CreateDepth();
}



void OffScreen::Draw()
{
	// 選択されたShaderModeに基づいて描画設定を実行
	switch (shaderMode_)
	{
	case ShaderMode::kNone:
		psoManager_->DrawCommonSetting(graphicsPipelineState[0], rootSignature[0]);
		srvManager_ = SrvManager::GetInstance();
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon->GetOffScreenGPUHandle());
		dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);
		break;
	case ShaderMode::kGray:
		psoManager_->DrawCommonSetting(graphicsPipelineState[1], rootSignature[0]);
		srvManager_ = SrvManager::GetInstance();
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon->GetOffScreenGPUHandle());
		dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);
		break;
	case ShaderMode::kVigneet:
		psoManager_->DrawCommonSetting(graphicsPipelineState[2], rootSignature[1]);
		srvManager_ = SrvManager::GetInstance();
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, vignetteResource->GetGPUVirtualAddress());
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon->GetOffScreenGPUHandle());
		dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);
		break;
	case ShaderMode::kSmooth:
		psoManager_->DrawCommonSetting(graphicsPipelineState[3], rootSignature[2]);
		srvManager_ = SrvManager::GetInstance();
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, smoothResource->GetGPUVirtualAddress());
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon->GetOffScreenGPUHandle());
		dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);
		break;
	case ShaderMode::kGauss:
		psoManager_->DrawCommonSetting(graphicsPipelineState[4], rootSignature[3]);
		srvManager_ = SrvManager::GetInstance();
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, gaussianResouce->GetGPUVirtualAddress());
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon->GetOffScreenGPUHandle());
		dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);
		break;
	case ShaderMode::kOutLine:
		psoManager_->DrawCommonSetting(graphicsPipelineState[5], rootSignature[0]);
		srvManager_ = SrvManager::GetInstance();
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon->GetOffScreenGPUHandle());
		dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);
		break;
	case ShaderMode::kDepth:
		psoManager_->DrawCommonSetting(graphicsPipelineState[6], rootSignature[4]);
		srvManager_ = SrvManager::GetInstance();
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, depthResouce->GetGPUVirtualAddress());
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon->GetDepthGPUHandle());
		dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);
		break;
	default:
		break;
	}

}

void OffScreen::DrawCommonSetting()
{
	ImGui::Begin("Offscreen");

	// ShaderModeを文字列で表現
	const char* shaderModeItems[] = { "None", "Gray", "Vignett", "Smooth", "Gauss", "OutLine" };
	int currentShaderMode = static_cast<int>(shaderMode_);

	// Comboを描画してユーザーが選択した場合に値を更新
	if (ImGui::Combo("Shader Mode", &currentShaderMode, shaderModeItems, IM_ARRAYSIZE(shaderModeItems)))
	{
		shaderMode_ = static_cast<ShaderMode>(currentShaderMode);
	}

	switch (shaderMode_)
	{
	case ShaderMode::kNone:
		break;
	case ShaderMode::kGray:
		break;
	case ShaderMode::kVigneet:
		ImGui::DragFloat("Exponent", &vignetteData->vignetteExponent, 0.1f,0.0f,10.0f);
		ImGui::DragFloat("Radius", &vignetteData->vignetteRadius, 0.01f,0.0f,10.0f);
		ImGui::DragFloat("Strength", &vignetteData->vignetteStrength, 0.01f);
		ImGui::DragFloat2("Center", &vignetteData->vignetteCenter.x, 0.01f,-10.0f,10.0f);
		break;
	case ShaderMode::kSmooth:
		ImGui::DragInt("Kernel Size", &smoothData->kernelSize, 2, 3, 7);
		break;
	case ShaderMode::kGauss:
		ImGui::DragInt("Kernel Size", &gaussianData->kernelSize, 2, 3, 7);
		ImGui::DragFloat("sigma", &gaussianData->sigma, 0.01f,0.01f,10.0f);
		break;
	case ShaderMode::kOutLine:
		break;
	case ShaderMode::kDepth:
		depthData->projectionInverse = Inverse(projectionInverse_);
		break;
	default:
		break;
	}

	ImGui::End();
}



void OffScreen::CreateSmooth()
{
	smoothResource = dxCommon->CreateBufferResource(sizeof(KernelSettings));
	smoothResource->Map(0, nullptr, reinterpret_cast<void**>(&smoothData));
	smoothData->kernelSize = 3;
}

void OffScreen::CreateGauss()
{
	gaussianResouce = dxCommon->CreateBufferResource(sizeof(GaussianParams));
	gaussianResouce->Map(0, nullptr, reinterpret_cast<void**>(&gaussianData));
	gaussianData->kernelSize = 3;
	gaussianData->sigma = 1;
}

void OffScreen::CreateVignette()
{
	vignetteResource = dxCommon->CreateBufferResource(sizeof(VignetteParameter));
	vignetteResource->Map(0, nullptr, reinterpret_cast<void**>(&vignetteData));
	vignetteData->vignetteExponent = 1.0f;
	vignetteData->vignetteRadius = 1.0f;
	vignetteData->vignetteStrength = 1.0f;
	vignetteData->vignetteCenter = { 0.5f,0.5f };
}

void OffScreen::CreateDepth()
{
	depthResouce = dxCommon->CreateBufferResource(sizeof(Material));
	depthResouce->Map(0, nullptr, reinterpret_cast<void**>(&depthData));
	depthData->projectionInverse = MakeIdentity4x4();
}

