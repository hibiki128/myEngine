#pragma once
#include "Sprite.h"
#include <imgui.h>  // ImGui用のヘッダ
#include "externals/nlohmann/json.hpp"
#include <string>
#include <vector>
using json = nlohmann::json;

class ScoreManager {
public:
	/// <summary>
	/// 数字スプライトを初期化する関数
	/// </summary>
	void Initialize();

	///// <summary>
	///// 更新
	///// </summary>
	// void Update(int newScore);

	/// <summary>
	/// // スコアを描画する関数
	/// </summary>
	void DrawScore(int score, Vector2 size, Vector2 pos);

	//void Draw();

	/// <summary>
	/// スコアをJSONファイルに保存
	/// </summary>
	/// <param name="filePath">保存するファイルのパス</param>
	/// <param name="scores">保存するスコアのリスト</param>
	void SaveScores(const std::string& filePath, const std::vector<int>& scores);

	/// <summary>
	/// JSONファイルからスコアを読み込み
	/// </summary>
	/// <param name="filePath">読み込むファイルのパス</param>
	/// <returns>読み込んだスコアのリスト</returns>
	std::vector<int> LoadScores(const std::string& filePath);
	int LoadScoresInt(const std::string& filePath);

	/// <summary>
	/// 新しいスコアを追加し、最大10個まで管理する
	/// </summary>
	/// <param name="filePath">保存するファイルのパス</param>
	/// <param name="newScore">新しいスコア</param>
	void AddScore(const std::string& filePath, int newScore);

	/// <summary>
	/// ImGuiを使ってトップ3のスコアを描画
	/// </summary>
	void DisplayTop3WithImGui(const std::string& filePath);
	Vector3 DisplayTop3(const std::string& filePath);

	/// <summary>
	/// スコアをリセット（全削除）
	/// </summary>
	void ResetScores(const std::string& filePath);

	/// <summary>
	/// トップ3を呼び出す
	/// </summary>
	Vector3 GetTop3() { return Vector3{ static_cast<float>(top1), static_cast<float>(top2), static_cast<float>(top3) }; }

	/// <summary>
	/// アンカーポイントの設定
	/// </summary>
	void SetAnchor(Vector2 anchor) {
		for (auto& spCopy : SpriteCopy) {
			spCopy.get()->SetAnchorPoint(anchor);
		}
	}

	void Setpos(Vector2 basePos) {
		// スプライト間の間隔
		float spacing = 10.0f; // 各数字の間隔を 10 ピクセルに設定
		float offsetX = 0.0f;  // 一番右のスプライトを基準にオフセット

		// スプライトリストの最後の要素（右端）から順に左に描画
		for (int i = static_cast<int>(SpriteCopy.size()) - 1; i >= 0; --i) {
			// 各スプライトの位置を計算
			float posX = basePos.x - offsetX; // 右から左にずらす
			SpriteCopy[i]->SetPosition(Vector2(posX, basePos.y));

			// 次のスプライトの位置をずらすためにオフセットを更新
			offsetX += SpriteCopy[i]->GetSize().x + spacing; // スプライトの幅と間隔分だけ左に移動
		}
	}

	void SetSize(Vector2 Size) {
		for (auto& spCopy : SpriteCopy) {
			spCopy.get()->SetSize(Size);
		}
	}
	void SetRotate(float Rotate) {
		for (auto& spCopy : SpriteCopy) {
			spCopy.get()->SetRotation(Rotate);
		}
	}

private:
	const int kMaxScores = 10; // 保存するスコアの最大数
	// トップ3
	int top1 = 0;
	int top2 = 0;
	int top3 = 0;
	int currentScore_ = -1; // 初期値は不正なスコア
	// 0〜9のスプライトを保持するための配列
	std::array<std::unique_ptr<Sprite>, 10> numberSprites_;
	std::vector<std::unique_ptr<Sprite>> SpriteCopy;
};
