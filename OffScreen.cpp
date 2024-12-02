#include "OffScreen.h"
#include"DirectXCommon.h"
#include"imgui.h"


void OffScreen::Initialize()
{
    dxCommon = DirectXCommon::GetInstance();
    psoManager_ = std::make_unique<PipeLineManager>();
    psoManager_->Initialize(dxCommon);
    rootSignature = psoManager_->CreateRenderRootSignature(rootSignature);
    graphicsPipelineState[0] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[0], rootSignature, ShaderMode::kNone);
    graphicsPipelineState[1] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[1], rootSignature, ShaderMode::kGray);
    graphicsPipelineState[2] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[2], rootSignature, ShaderMode::kVigneet);
    graphicsPipelineState[3] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[3], rootSignature, ShaderMode::kSmooth);
    graphicsPipelineState[4] = psoManager_->CreateRenderGraphicsPipeLine(graphicsPipelineState[4], rootSignature, ShaderMode::kGauss);
}



void OffScreen::Draw()
{
    // 選択されたShaderModeに基づいて描画設定を実行
    switch (shaderMode_)
    {
    case ShaderMode::kNone:
        psoManager_->DrawCommonSetting(graphicsPipelineState[0], rootSignature);
        break;
    case ShaderMode::kGray:
        psoManager_->DrawCommonSetting(graphicsPipelineState[1], rootSignature);
        break;
    case ShaderMode::kVigneet:
        psoManager_->DrawCommonSetting(graphicsPipelineState[2], rootSignature);
        break;
    case ShaderMode::kSmooth:
        psoManager_->DrawCommonSetting(graphicsPipelineState[3], rootSignature);
        break;
    case ShaderMode::kGauss:
        psoManager_->DrawCommonSetting(graphicsPipelineState[4], rootSignature);
        break;
    default:
        break;
    }
    srvManager_ = SrvManager::GetInstance();
    dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon->GetOffScreenGPUHandle());
    dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void OffScreen::DrawCommonSetting()
{
    ImGui::Begin("Offscreen");

    // ShaderModeを文字列で表現
    const char* shaderModeItems[] = { "None", "Gray","Vignett","Smooth","Gauss"};
    int currentShaderMode = static_cast<int>(shaderMode_);

    // Comboを描画してユーザーが選択した場合に値を更新
    if (ImGui::Combo("Shader Mode", &currentShaderMode, shaderModeItems, IM_ARRAYSIZE(shaderModeItems)))
    {
        shaderMode_ = static_cast<ShaderMode>(currentShaderMode);
    }

    ImGui::End();
}


