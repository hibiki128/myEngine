#include "cassert"
#include "ModelManager.h"
#include "myMath.h"
#include "Object3d.h"
#include"Object3dCommon.h"
#include <line/DrawLine3D.h>




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
}

void Object3d::Update(const WorldTransform& worldTransform, const ViewProjection& viewProjection)
{

	cameraForGPUData->worldPosition = viewProjection.translation_;
	Matrix4x4 worldMatrix = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);

	if (worldTransform.parent_) {
		worldMatrix *= worldTransform.parent_->matWorld_;
	}
	Matrix4x4 worldViewProjectionMatrix;
	const Matrix4x4& viewProjectionMatrix = viewProjection.matView_ * viewProjection.matProjection_;
	worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
	Matrix4x4 worldInverseMatrix = Inverse(worldMatrix);
	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;
	transformationMatrixData->WorldInverseTranspose = Transpose(worldInverseMatrix);

	if (model) {
		model->Update();
	}

}

void Object3d::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, ObjColor* color, bool Lighting)
{

	//if (viewProjection.IsOutsideViewFrustum(worldTransform)) {
	//	return;
	//}

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

void Object3d::DrawSkelton(const WorldTransform& worldTransform,const ViewProjection& viewProjection)
{
	Update(worldTransform, viewProjection);
	// スケルトンデータを取得
	const Model::Skeleton& skeleton = model->GetSkeletonData();

	// 各ジョイントを巡回して親子関係の線を生成
	for (const auto& joint : skeleton.joints) {
		// 親がいない場合、このジョイントはルートなのでスキップ
		if (!joint.parent.has_value()) {
			continue;
		}

		// 親ジョイントを取得
		const auto& parentJoint = skeleton.joints[*joint.parent];

		// 親と子のスケルトン空間座標を取得
		Vector3 parentPosition = ExtractTranslation(parentJoint.skeltonSpaceMatrix);
		Vector3 childPosition = ExtractTranslation(joint.skeltonSpaceMatrix);

		// 線の色を設定（デフォルトで白色）
		Vector4 lineColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		// LineManagerに現在の線分を登録
		DrawLine3D::GetInstance()->SetPoints(parentPosition, childPosition, lineColor);
	}

	DrawLine3D::GetInstance()->Draw(viewProjection);
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

