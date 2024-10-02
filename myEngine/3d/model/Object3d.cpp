#include "cassert"
#include "ModelManager.h"
#include "myMath.h"
#include "Object3d.h"
#include"Object3dCommon.h"



void Object3d::Initialize(const std::string& filePath)
{
	this->obj3dCommon = Object3dCommon::GetInstance();

	CreateTransformationMatrix();

	CreateDirectionLight();

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

	if (model) {
		model->Update();
	}

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 worldViewProjectionMatrix;
	const Matrix4x4& viewProjectionMatrix = viewProjection.matView_ * viewProjection.matProjection_;
	worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;

	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;

}

void Object3d::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection)
{
	Update(worldTransform, viewProjection);

	// wvp用のCBufferの場所を設定
	obj3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
	// DirectionalLight用のCBufferの場所を設定
	obj3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	if (model) {
		model->Draw();
	}
}

void Object3d::SetModel(const std::string& filePath)
{
	// モデルを検索してセットする
	model = ModelManager::GetInstance()->FindModel(filePath);
}

void Object3d::CreateTransformationMatrix()
{
	transformationMatrixResource = obj3dCommon->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	// 書き込むかめのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	// 単位行列を書き込んでおく
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
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
}
