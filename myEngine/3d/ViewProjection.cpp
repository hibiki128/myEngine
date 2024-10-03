#include "ViewProjection.h"

void ViewProjection::Initialize()
{
	matView_ = MakeIdentity4x4();
	matProjection_ = MakeIdentity4x4();

	dxCommon_ = DirectXCommon::GetInstance();

	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void ViewProjection::CreateConstBuffer()
{
	const UINT bufferSize = sizeof(ConstBufferDataViewProjection);
	constBuffer_ = dxCommon_->CreateBufferResource(bufferSize);
}

void ViewProjection::Map()
{
	HRESULT hr = constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
	if (FAILED(hr)) {
		// エラーハンドリング

	}
}

void ViewProjection::UpdateMatrix()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();

	TransferMatrix();
}

void ViewProjection::TransferMatrix()
{

	matView_ = Inverse(MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotation_, translation_));
	matProjection_ = MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);

	if (constMap) {
		constMap->view = matView_;
		constMap->projection = matProjection_;
	}
}

void ViewProjection::UpdateViewMatrix()
{
	matView_ = Inverse(MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotation_, translation_));
}

void ViewProjection::UpdateProjectionMatrix()
{
	matProjection_ = MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}
