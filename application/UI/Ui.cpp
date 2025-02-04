#include "Ui.h"
#include "Easing.h"
#ifdef _DEBUG 
#include "imgui.h"
#endif // _DEBUG
#include <filesystem>
#include <fstream>

void UI::Init() {
    explanation_ = std::make_unique<Sprite>();
    explanation_->Initialize("game/explanation.png", spPos_, {1, 1, 1, 1}, {0.5f, 0.5f});
    explanation_->SetSize(explanation_->GetSize() / 1.25f);
    explanation2_ = std::make_unique<Sprite>();
    explanation2_->Initialize("game/explanation2.png", spPos2_, {1, 1, 1, 1}, {0.5f, 0.5f});
    explanation2_->SetSize(explanation2_->GetSize() / 1.25f);

    singleDigit_ = std::make_unique<Sprite>();
    singleDigit_->Initialize("game/0.png", spPos4_, {1, 1, 1, 1}, {0.5f, 0.5f});
    singleDigit_->SetSize(singleDigit_->GetSize() / 1.25f);
    twoDigit_ = std::make_unique<Sprite>();
    twoDigit_->Initialize("game/0.png", spPos5_, {1, 1, 1, 1}, {0.5f, 0.5f});
    twoDigit_->SetSize(twoDigit_->GetSize() / 1.25f);

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
    Number();
    singleDigit_->SetPosition(spPos4_);
    twoDigit_->SetPosition(spPos5_);
    if (isStart_) {
        TextMove();
    }
}

void UI::Draw() {
    explanation_->Draw();
    explanation2_->Draw();
    singleDigit_->Draw();
    if (enemyNum > 9) {
        twoDigit_->Draw();
    }
    game_->Draw();
}

void UI::Debug() {
#ifdef _DEBUG
    ImGui::Begin("UI");
    ImGui::DragFloat2("位置", &spPos_.x, 1.0f);
    ImGui::DragFloat2("位置2", &spPos2_.x, 1.0f);
    ImGui::DragFloat2("位置3", &spPos4_.x, 1.0f);
    ImGui::DragFloat2("位置4", &spPos5_.x, 1.0f);

    if (ImGui::Button("セーブ")) {
        SaveToJson();
        std::string message = std::format("UI saved.");
        MessageBoxA(nullptr, message.c_str(), "UI", 0);
    }

    ImGui::End();
#endif // _DEBUG
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
    j["translation3"] = {spPos4_.x, spPos4_.y};
    j["translation4"] = {spPos5_.x, spPos5_.y};

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
    spPos4_ = {j["translation3"][0], j["translation3"][1]};
    spPos5_ = {j["translation4"][0], j["translation4"][1]};
}

void UI::Number() {
    // 10の位と1の位を取得
    int tens = enemyNum / 10; // 10の位
    int ones = enemyNum % 10; // 1の位

    // 1桁の場合は10の位を0にする
    std::string tensTexture = "game/" + std::to_string(tens) + ".png";
    std::string onesTexture = "game/" + std::to_string(ones) + ".png";

    // テクスチャを設定
    twoDigit_->SetTexturePath(tensTexture);
    singleDigit_->SetTexturePath(onesTexture);
}
