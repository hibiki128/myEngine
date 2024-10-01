#pragma once
#include "cstdint"
#include "string"
#include "vector"
#include "xaudio2.h"
#include"wrl.h"
#include"array"
#include <set>

class Audio
{
private:
	static const int kMaxSoundData = 256;

	static Audio* instance;

	Audio() = default;
	~Audio() = default;
	Audio(Audio&) = default;
	Audio& operator=(Audio&) = default;
private:

	// チャンクヘッダー
	struct ChunkHeader
	{
		char id[4]; // チャンク等のID
		int32_t size; // チャンクサイズ
	};

	// RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk; // "RIFF"
		char type[4]; // "WAVE"
	};

	// FMTチャンク
	struct FormatChunk
	{
		ChunkHeader chunk; // "fmt"
		WAVEFORMATEX fmt; // 波形フォーマット
	};

	// 音声データ
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファ
		std::vector<uint8_t> buffer;
		// 名前
		std::string name_;
	};

	// 再生データ
	struct Voice {
		uint32_t handle = 0u; // 音声ハンドル
		IXAudio2SourceVoice* sourceVoice = nullptr; // ソースボイス
		float volume = 1.0f; // 音量 (0.0 ～ 1.0)
	};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directoryPath"></param>
	void Initialize(const std::string& directoryPath = "resources/sounds");

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Audio* GetInstance();

	/// <summary>
	/// 音声読み込み
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	uint32_t LoadWave(const char* filename);

	/// <summary>
	/// 音声データ解放
	/// </summary>
	/// <param name="soundData"></param>
	void Unload(uint32_t soundIndex);

	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="xAudio2"></param>
	/// <param name="soundData"></param>
	void PlayWave(uint32_t soundIndex, float volume);

	/// <summary>
	/// 音声停止
	/// </summary>
	/// <param name="soundIndex"></param>
	void StopWave(uint32_t soundIndex);

	/// <summary>
	/// 音量設定
	/// </summary>
	/// <param name="soundIndex"></param>
	/// <param name="volume"></param>
	void SetVolume(uint32_t soundIndex, float volume);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

private:

	Microsoft::WRL::ComPtr<IXAudio2>xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	std::string directoryPath_;
	std::array<SoundData, kMaxSoundData> soundDatas_;
	size_t soundDataIndex = 0;  // 次に保存する音声データのインデックス
	std::set<Voice*> voices_; // 再生中の音声データを管理するセット
	std::set<std::string> loadedFiles; // 読み込まれたファイルのセット
};

