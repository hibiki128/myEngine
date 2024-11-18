#define NOMINMAX
#include "ScoreManager.h"
#include "TextureManager.h"
#include <algorithm>
#include <fstream>
#include <iostream>

void ScoreManager::Initialize() {
	// 0～9の数字に対応するスプライトを初期化する
	for (int i = 0; i < 10; ++i) {
		std::string texturePath = std::to_string(i) + ".png";
		numberSprites_[i] = std::make_unique<Sprite>();
		numberSprites_[i]->Initialize(texturePath, Vector2(0, 0));
	}
}

/// <summary>
///  メモリリークあり
/// </summary>
/// <param name="score"></param>
/// <param name="x"></param>
/// <param name="y"></param>
void ScoreManager::DrawScore(int score, Vector2 size, Vector2 pos) {
	// スコアが変更されている場合にのみ、スプライトを更新
	if (score != currentScore_) {
		// スコアが変更されている場合、古いスプライトを削除
		SpriteCopy.clear(); // 現在のスプライトをすべて削除

		// スコアの更新
		currentScore_ = score;

		// スコアの描画処理
		std::string scoreStr = std::to_string(score);
		size_t numDigits = scoreStr.size();

		// 全体のスコアの幅を計算する
		float totalWidth = size.x * numDigits;

		// スプライトの初期位置を右揃えに調整
		pos.x -= totalWidth;

		// スプライトを生成し、リストに追加
		for (size_t i = 0; i < numDigits; ++i) {
			char c = scoreStr[i];
			int digit = c - '0';

			if (digit >= 0 && digit <= 9) {
				// 新しいスプライトを作成
				std::unique_ptr<Sprite> spriteCopy;
				spriteCopy = std::make_unique<Sprite>();
				spriteCopy->Initialize(numberSprites_[digit]->GetFilePath(), numberSprites_[digit]->GetSize());

				// スプライトの実際のサイズ
				Vector2 originalSize = numberSprites_[digit]->GetSize();

				// スプライトのスケーリング比率を計算
				float scaleX = size.x / originalSize.x;
				float scaleY = size.y / originalSize.y;

				// スプライトにスケールを適用
				spriteCopy->SetSize(originalSize * Vector2(scaleX, scaleY));

				// スプライトの位置を計算
				Vector2 spritePos = pos;

				// スプライトの位置を設定
				spriteCopy->SetPosition(spritePos);

				// 新しいスプライトをリストに追加
				SpriteCopy.push_back(std::move(spriteCopy));

				// 次のスプライトの位置を計算
				pos.x += size.x; // 桁ごとに右にずらす
			}
		}
	}

	// スコアのスプライトを描画する処理
	if (!SpriteCopy.empty()) {
		for (size_t i = 0; i < SpriteCopy.size(); ++i) {
			// スプライトの描画処理
			SpriteCopy[i]->Draw();
		}
	}
	else {
		// スプライトが空の場合、何も描画しない
	}
}


/// <summary>
/// スコアをJSONファイルに保存
/// </summary>
void ScoreManager::SaveScores(const std::string& filePath, const std::vector<int>& scores) {
	json j_scores = scores;
	std::ofstream file(filePath);
	if (file.is_open()) {
		file << j_scores.dump(4); // インデント付きで保存
		file.close();
	}
	else {
		std::cerr << "Failed to open file for saving: " << filePath << std::endl;
	}
}

/// <summary>
/// JSONファイルからスコアを読み込み
/// </summary>
std::vector<int> ScoreManager::LoadScores(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file for reading: " << filePath << std::endl;
		return {};
	}

	json j_scores;
	try {
		file >> j_scores;
	}
	catch (json::parse_error& e) {
		std::cerr << "JSON parse error: " << e.what() << std::endl;
		return {};
	}

	return j_scores.get<std::vector<int>>();
}
int ScoreManager::LoadScoresInt(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file for reading: " << filePath << std::endl;
		return -1; // エラー時の返り値
	}

	json j_scores;
	try {
		file >> j_scores;
	}
	catch (json::parse_error& e) {
		std::cerr << "JSON parse error: " << e.what() << std::endl;
		return -1; // エラー時の返り値
	}

	// スコアが配列として含まれていると仮定
	std::vector<int> scores = j_scores.get<std::vector<int>>();
	if (scores.empty()) {
		std::cerr << "No scores found in file" << std::endl;
		return -1; // スコアが見つからない場合
	}

	return scores[0]; // 最初のスコアを返す
}

/// <summary>
/// 新しいスコアを追加し、最大10個まで管理する
/// </summary>
void ScoreManager::AddScore(const std::string& filePath, int newScore) {
	std::vector<int> scores = LoadScores(filePath);

	// スコアを追加
	scores.push_back(newScore);

	// スコアを降順にソート
	std::sort(scores.begin(), scores.end(), std::greater<int>());

	// スコアが10個を超えたら、低いスコアを削除
	if (scores.size() > kMaxScores) {
		scores.resize(kMaxScores); // 上位10個のみ保持
	}

	// スコアを保存
	SaveScores(filePath, scores);
}

/// <summary>
/// ImGuiを使ってトップ3のスコアを描画
/// </summary>
void ScoreManager::DisplayTop3WithImGui(const std::string& filePath) {
	std::vector<int> scores = LoadScores(filePath);

	ImGui::Begin("Top 3 Scores");

	ImGui::Text("Top 3 Scores:");
	for (int i = 0; i < std::min(3, static_cast<int>(scores.size())); ++i) {
		ImGui::Text("%d: %d", i + 1, scores[i]);
	}

	ImGui::End();
}

Vector3 ScoreManager::DisplayTop3(const std::string& filePath) {
	std::vector<int> scores = LoadScores(filePath);

	// スコアが少なくとも3つあるか確認し、足りない場合は0で埋める
	top1 = scores.size() > 0 ? scores[0] : 0;
	top2 = scores.size() > 1 ? scores[1] : 0;
	top3 = scores.size() > 2 ? scores[2] : 0;

	return Vector3{ static_cast<float>(top1), static_cast<float>(top2), static_cast<float>(top3) };
}

/// <summary>
/// スコアをリセット（全削除）
/// </summary>
void ScoreManager::ResetScores(const std::string& filePath) {
	std::vector<int> emptyScores;
	SaveScores(filePath, emptyScores); // 空のスコアを保存してリセット
}
