#include "Ui.h"
#include"imgui.h"
#include <fstream>
#include <filesystem>
void UI::Init()
{
	explanation_ = std::make_unique<Sprite>();
	explanation_->Initialize("explanation.png", spPos_, { 1,1,1,1 }, { 0.5f,0.5f });
	explanation_->SetSize(explanation_->GetSize() / 1.25f);
	LoadFromJson();
}

void UI::Update()
{
	explanation_->SetPosition(spPos_);
}

void UI::Draw()
{
	explanation_->Draw();
}

void UI::Debug()
{
	ImGui::Begin("UI");
	ImGui::DragFloat2("位置", &spPos_.x, 1.0f);

	if (ImGui::Button("セーブ")) {
		SaveToJson();
		std::string message = std::format("UI saved.");
		MessageBoxA(nullptr, message.c_str(), "UI", 0);
	}

	ImGui::End();
}

void UI::SaveToJson()
{
	json j;
	
	j["translation"] = { spPos_.x,spPos_.y };

	// ディレクトリを作成し、JSONファイルを保存
	std::filesystem::create_directories("resources/jsons/UI/");
	std::ofstream outFile("resources/jsons/UI/UI.json");
	outFile << j.dump(4);
}

void UI::LoadFromJson()
{
	std::ifstream inFile("resources/jsons/UI/UI.json");
	if (!inFile.is_open()) {
		return; // JSONファイルがない場合は早期リターン
	}

	json j;
	inFile >> j;

	spPos_ = { j["translation"][0],j["translation"][1] };
}
