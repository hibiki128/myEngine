#include "HitStop.h"
#include <chrono>

void HitStop::Initialize() {
    LoadSettings();
}

void HitStop::Update() {
    if (!isActive_)
        return;

    elapsedTime_ += 1.0f / 60.0f; // 仮に60FPSとして進める
    if (elapsedTime_ >= stopDuration_) {
        isActive_ = false;
        elapsedTime_ = 0.0f;
    }
}

void HitStop::Start(float duration) {
    stopDuration_ = duration;
    elapsedTime_ = 0.0f;
    isActive_ = true;
}

void HitStop::LoadSettings() {
    std::ifstream file("resources/jsons/HitStop/hitstop.json");
    if (!file)
        return;

    nlohmann::json json;
    file >> json;
    file.close();

    stopDuration_ = json["stopDuration"];
}

void HitStop::SaveSettings() {
    nlohmann::json json;
    json["stopDuration"] = stopDuration_;

    std::ofstream file("resources/jsons/HitStop/hitstop.json");
    file << json.dump(4);
    file.close();
}

void HitStop::imgui() {
#ifdef _DEBUG
    if (ImGui::Begin("ヒットストップ設定")) {
        ImGui::DragFloat("停止時間 (秒)", &stopDuration_, 0.01f, 0.01f, 5.0f);

        if (ImGui::Button("設定を保存")) {
            SaveSettings();
        }

        if (ImGui::Button("ヒットストップ開始")) {
            Start(stopDuration_);
        }
    }
    ImGui::End();
#endif
}
