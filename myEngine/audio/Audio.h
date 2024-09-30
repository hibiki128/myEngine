#pragma once
#include "xaudio2.h"
#include "cstdint"

class Audio
{
private:
	static Audio* instance;

	Audio() = default;
	~Audio() = default;
	Audio(Audio&) = default;
	Audio& operator=(Audio&) = default;

public:
	/// <summary>
	/// 音声読み込み
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	SoundData SoundLoadWave(const char* filename);

	/// <summary>
	/// 音声データ解放
	/// </summary>
	/// <param name="soundData"></param>
	void SoundUnload(SoundData* soundData);

	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="xAudio2"></param>
	/// <param name="soundData"></param>
	void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);
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

	struct SoundData
	{
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		BYTE* pBuffer;
		// バッファのサイズ
		unsigned int bufferSize;
	};

};

