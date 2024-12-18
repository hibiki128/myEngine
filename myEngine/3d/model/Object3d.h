#pragma once
#include"d3d12.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "string"
#include "vector"
#include "Vector2.h" 
#include "Vector3.h"
#include "Vector4.h"
#include"WorldTransform.h"
#include"wrl.h"
#include"ViewProjection.h"
#include"ObjColor.h"

enum class LightType {
	Directional,
	Point,
};

class ModelCommon;
class Object3dCommon;
class Object3d
{
private: // メンバ変数

	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	// 座標変換行列データ
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInverseTranspose;
	};

	// 平行光源データ
	struct DirectionLight {
		Vector4 color; //!< ライトの色
		Vector3 direction; //!< ライトの向き
		float intensity;//!< 輝度
		int32_t active;
	};

	// マテリアルデータ
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		float shininess;
	};

	struct CameraForGPU {
		Vector3 worldPosition;
	};

	struct PointLight {
		Vector4 color;
		Vector3 position;
		float intensity;
		int32_t active;
		float radius;
		float decay;
	};

	Object3dCommon* obj3dCommon = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	// バッファリソース内のデータを指すポインタ
	DirectionLight* directionalLightData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource;
	// バッファリソース内のデータを指すポインタ
	PointLight* pointLightData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	// バッファリソース内のデータを指すポインタ
	Material* materialData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraForGPUResource;
	// バッファリソース内のデータを指すポインタ
	CameraForGPU* cameraForGPUData = nullptr;

	Transform transform;
	//Transform cameraTransform;

	Model* model = nullptr;
	ModelCommon* modelCommon = nullptr;

	// 移動させる用各SRT
	Vector3 position = { 0.0f,0.0f,0.0f };
	Vector3 rotation = { 0.0f,0.0f,0.0f };
	Vector3 size = { 1.0f,1.0f,1.0f };

public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::string& filePath);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const WorldTransform& worldTransform, const ViewProjection& viewProjection);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, ObjColor* color = nullptr, bool Lighting = true);

	/// <summary>
	/// getter
	/// </summary>
	/// <returns></returns>
	const Vector3& GetPosition()const { return position; }
	const Vector3& GetRotation() const { return rotation; }
	const Vector3& GetSize() const { return size; }

	/// <summary>
	/// setter
	/// </summary>
	/// <param name="position"></param>
	void SetModel(Model* model) { this->model = model; }
	void SetPosition(const Vector3& position) { this->position = position; }
	void SetRotation(const Vector3& rotation) { this->rotation = rotation; }
	void SetSize(const Vector3& size) { this->size = size; }
	void SetModel(const std::string& filePath);

	/// <summary>
	/// 平行光源の設定
	/// </summary>
	/// <param name="color">ライトの色</param>
	/// <param name="direction">ライトの向き</param>
	/// <param name="intensity">ライトの輝度</param>
	void SetDirectionalLight
	(Vector3 direction = { 0.0f,-1.0f,0.0f }, float intensity = { 1.0f }, Vector4 color = { 1.0f,1.0f,1.0f,1.0f });

	/// <summary>
	/// 点光源の設定
	/// </summary>
	/// <param name="position"></param>
	/// <param name="intensity"></param>
	/// <param name="color"></param>
	void SetPointLight(Vector3 position = { 0.0f,-1.0f,0.0f }, float intensity = { 1.0f }, Vector4 color = { 1.0f,1.0f,1.0f,1.0f },float decay = 1.0f,float radius = 2.0f);

	/// <summary>
	/// ライトの種類設定
	/// </summary>
	/// <param name="lightType"></param>
	void SetLightType(LightType lightType = LightType::Directional);

	/// <summary>
	/// 光沢度の設定
	/// </summary>
	/// <param name="shininess">マテリアルの光沢度</param>
	void SetShininess(float shininess = 20.0f);

private: // メンバ関数

	/// <summary>
	/// 座標変換行列データ作成
	/// </summary>
	void CreateTransformationMatrix();

	/// <summary>
	/// 平行光源データ作成
	/// </summary>
	void CreateDirectionLight();

	/// <summary>
	/// マテリアルデータ作成
	/// </summary>
	void CreateMaterial();

	/// <summary>
	/// カメラ作成
	/// </summary>
	void CreateCamera();

	/// <summary>
	/// 点光源データ作成
	/// </summary>
	void CreatePointLight();
};

