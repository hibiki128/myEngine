#pragma once
#include"d3d12.h"
#include"Vector4.h"
#include"Vector2.h"
#include"Vector3.h"
#include "Matrix4x4.h"
#include"wrl.h"
#include"string"
#include"SrvManager.h"
class SpriteCommon;
class Sprite
{
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SpriteCommon* spriteCommon, std::string textureFilePath);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// getter
	/// </summary>
	/// <returns></returns>
	const Vector2& GetPosition()const { return position; }
	float GetRotation() const { return rotation; }
	const Vector2& GetSize() const { return size; }
	const Vector4& GetColor()const { return materialData->color; }
	const Vector2& GetAnchorPoint()const { return anchorPoint; }
	const bool GetFlipX()const { return isFlipX_; }
	const bool GetFilpY()const { return isFlipY_; }
	const Vector2& GetTexLeftTop()const { return textureLeftTop; }
	const Vector2& GetTexSize()const { return textureSize; }

	/// <summary>
	/// setter
	/// </summary>
	/// <param name="position"></param>
	void SetPosition(const Vector2& position) { this->position = position; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetSize(const Vector2& size) { this->size = size; }
	void SetColor(const Vector4& color) { materialData->color = color; }
	void SetTexturePath(std::string textureFilePath);
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }
	void SetFlipX(bool isFlipX) { isFlipX_ = isFlipX; }
	void SetFlipY(bool isFlipY) { isFlipY_ = isFlipY; }
	void SetTexLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }
	void SetTexSize(const Vector2& textureSize) { this->textureSize = textureSize; }

private: // メンバ関数

	/// <summary>
	/// 頂点データ作成
	/// </summary>
	void CreateVartexData();

	/// <summary>
	/// マテリアルデータ作成
	/// </summary>
	void CreateMaterial();

	/// <summary>
	/// 座標変換行列データ作成
	/// </summary>
	void CreateTransformationMatrix();

	/// <summary>
	/// テクスチャサイズをイメージに合わせる
	/// </summary>
	void AdjustTextureSize();
private:

	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	// 頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	// マテリアルデータ
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	// 座標変換行列データ
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	SpriteCommon* spriteCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	// バッファリソース内のデータを指すポインタ
	Material* materialData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	// 移動させる用各SRT
	Vector2 position = { 0.0f,0.0f };
	float rotation = 0.0f;
	Vector2 size = { 640.0f,360.0f };

	std::string directoryPath_ = "resources/images";
	std::string fullpath;
	Vector2 anchorPoint = { 0.0f,0.0f };

	// 左右フリップ
	bool isFlipX_ = false;

	// 上下フリップ
	bool isFlipY_ = false;

	// テクスチャ左上座標
	Vector2 textureLeftTop = { 0.0f,0.0f };
	// テクスチャ切り出しサイズ
	Vector2 textureSize = { 512.0f,512.0f };

};

