#include "cassert"
#include "ModelManager.h"
#include "myMath.h"
#include "Object3d.h"
#include"Object3dCommon.h"



void Object3d::Initialize(const std::string& filePath)
{
	this->obj3dCommon = Object3dCommon::GetInstance();

	CreateTransformationMatrix();

	CreateMaterial();

	if (materialData->enableLighting != 0) {
		CreateCamera();

		CreateDirectionLight();

		CreatePointLight();
	}

	ModelManager::GetInstance()->LoadModel(filePath);

	// モデルを検索してセットする
	model = ModelManager::GetInstance()->FindModel(filePath);

	// モデルを検索してセットする
	model = ModelManager::GetInstance()->FindModel(filePath);

	// Transform変数を作る
	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
}

void Object3d::Update(const WorldTransform& worldTransform, const ViewProjection& viewProjection)
{
	transform.translate = worldTransform.translation_;
	transform.rotate = worldTransform.rotation_;
	transform.scale = worldTransform.scale_;

	cameraForGPUData->worldPosition = viewProjection.translation_;

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 worldViewProjectionMatrix;
	const Matrix4x4& viewProjectionMatrix = viewProjection.matView_ * viewProjection.matProjection_;
	worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;

	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;
	Matrix4x4 worldInverseMatrix = Inverse(worldMatrix);
	transformationMatrixData->WorldInverseTranspose = Transpose(worldInverseMatrix);

}

<<<<<<< HEAD
void Object3d::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, ObjColor* color, bool Lighting)
{
	if (color) {
		materialData->color = color->GetColor();
	}
	materialData->enableLighting = Lighting;
	if (!Lighting) {
		directionalLightData->active = false;
		pointLightData->active = false;
	}
	Update(worldTransform, viewProjection);

	obj3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
=======
void Object3d::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection)
{
	Update(worldTransform, viewProjection);

>>>>>>> 588d19cc7ec69ea69ec2b4e7b1eb2ed8ab6bceb3
	// wvp用のCBufferの場所を設定
	obj3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
	if (materialData->enableLighting != 0) {
		// DirectionalLight用のCBufferの場所を設定
		obj3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

		obj3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraForGPUResource->GetGPUVirtualAddress());

		obj3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());
	}
	// マテリアルCBufferの場所を設定
	if (model) {
		model->Draw();
	}
}

void Object3d::SetModel(const std::string& filePath)
{
	// モデルを検索してセットする
	model = ModelManager::GetInstance()->FindModel(filePath);
}

void Object3d::SetDirectionalLight(Vector3 direction, float intensity, Vector4 color)
{
	directionalLightData->color = color;
	directionalLightData->direction = direction;
	directionalLightData->intensity = intensity;
}

void Object3d::SetPointLight(Vector3 position, float intensity, Vector4 color, float decay, float radius)
{
	pointLightData->color = color;
	pointLightData->position = position;
	pointLightData->intensity = intensity;
	pointLightData->decay = decay;
	pointLightData->radius = radius;
}

void Object3d::SetLightType(LightType lightType)
{
	if (lightType == LightType::Directional) {
		directionalLightData->active = true;
		pointLightData->active = false;
	}
	else if (lightType == LightType::Point) {
		directionalLightData->active = false;
		pointLightData->active = true;
	}
}

void Object3d::SetShininess(float shininess)
{
	materialData->shininess = shininess;
}

void Object3d::CreateTransformationMatrix()
{
	transformationMatrixResource = obj3dCommon->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	// 書き込むかめのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	// 単位行列を書き込んでおく
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
	transformationMatrixData->WorldInverseTranspose = MakeIdentity4x4();
}

void Object3d::CreateDirectionLight()
{
	directionalLightResource = obj3dCommon->GetDxCommon()->CreateBufferResource(sizeof(DirectionLight));
	// 書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	// デフォルト値
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;
	directionalLightData->active = true;
}

void Object3d::CreateMaterial()
{
	// Sprite用のマテリアルリソースをつくる
	materialResource = obj3dCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色の設定
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// Lightingの設定
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();
	materialData->shininess = 20.0f;
}

void Object3d::CreateCamera()
{
	cameraForGPUResource = obj3dCommon->GetDxCommon()->CreateBufferResource(sizeof(CameraForGPU));
	cameraForGPUResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPUData));
	cameraForGPUData->worldPosition = { 0.0f,0.0f,-50.0f };
}

void Object3d::CreatePointLight()
{
	pointLightResource = obj3dCommon->GetDxCommon()->CreateBufferResource(sizeof(PointLight));
	// 書き込むためのアドレスを取得
	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));
	// デフォルト値
	pointLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData->position = { -1.0f,4.0f,-3.0f };
	pointLightData->intensity = 10.0f;
	pointLightData->decay = 1.0f;
	pointLightData->radius = 2.0f;
	pointLightData->active = false;
}

