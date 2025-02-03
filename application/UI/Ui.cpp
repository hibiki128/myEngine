#include "Ui.h"
#include "Easing.h"
#include "imgui.h"
#include <filesystem>
#include <fstream>

void UI::Init() {
    explanation_ = std::make_unique<Sprite>();
    explanation_->Initialize("game/explanation.png", spPos_, {1, 1, 1, 1}, {0.5f, 0.5f});
    explanation_->SetSize(explanation_->GetSize() / 1.25f);
    explanation2_ = std::make_unique<Sprite>();
    explanation2_->Initialize("game/explanation2.png", spPos2_, {1, 1, 1, 1}, {0.5f, 0.5f});
    explanation2_->SetSize(explanation2_->GetSize() / 1.25f);

    game_ = std::make_unique<Sprite>();
    game_->Initialize("game/gameClear.png", spPos3_, {1, 1, 1, 1}, {0.5f, 0.5f});
    game_->SetSize(game_->GetSize() / 1.25f);

    spPos3_ = {-300.0f, 360.0f};

    LoadFromJson();
}

void UI::Update() {
    explanation_->SetPosition(spPos_);
    explanation2_->SetPosition(spPos2_);
    game_->SetPosition(spPos3_);
    if (isStart_) {
        TextMove();
    }
}

void UI::Draw() {
    explanation_->Draw();
    explanation2_->Draw();
    game_->Draw();
}

void UI::Debug() {
    ImGui::Begin("UI");
    ImGui::DragFloat2("位置", &spPos_.x, 1.0f);
    ImGui::DragFloat2("位置2", &spPos2_.x, 1.0f);

    if (ImGui::Button("セーブ")) {
        SaveToJson();
        std::string message = std::format("UI saved.");
        MessageBoxA(nullptr, message.c_str(), "UI", 0);
    }

    ImGui::End();
}

void UI::TextMove() {
    const float startPos = -640.0f;
    const float endPos = 640.0f;
    const float easeTMax = 1.0f;

    if (t_ < easeTMax) {
        t_ += 1.0f / 60.0f;
    } else {
        t_ = easeTMax;
        isFinish_ = true;
    }

    spPos3_.x = EaseInCubic<float>(startPos, endPos, t_, easeTMax);
}

void UI::SaveToJson() {
    json j;

    j["translation"] = {spPos_.x, spPos_.y};
    j["translation2"] = {spPos2_.x, spPos2_.y};

    // ディレクトリを作成し、JSONファイルを保存
    std::filesystem::create_directories("resources/jsons/UI/");
    std::ofstream outFile("resources/jsons/UI/UI.json");
    outFile << j.dump(4);
}

void UI::LoadFromJson() {
    std::ifstream inFile("resources/jsons/UI/UI.json");
    if (!inFile.is_open()) {
        return; // JSONファイルがない場合は早期リターン
    }

    json j;
    inFile >> j;

    spPos_ = {j["translation"][0], j["translation"][1]};
    spPos2_ = {j["translation2"][0], j["translation2"][1]};
}
