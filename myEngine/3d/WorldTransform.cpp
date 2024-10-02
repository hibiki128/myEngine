#include "WorldTransform.h"

void WorldTransform::Initialize()
{
	// �X�P�[���A��]�A���s�ړ���������
	scale_ = { 1.0f, 1.0f, 1.0f }; // �f�t�H���g�̃X�P�[��
	rotation_ = { 0.0f, 0.0f, 0.0f }; // �f�t�H���g�̉�]
	translation_ = { 0.0f, 0.0f, 0.0f }; // �f�t�H���g�̈ʒu
	dxCommon_ = DirectXCommon::GetInstance();
	// �s��̏�����
	matWorld_ = MakeIdentity4x4(); // �P�ʍs��ŏ�����

	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void WorldTransform::TransferMatrix()
{
	// ���[���h�s����v�Z
	matWorld_ = MakeAffineMatrix(scale_, rotation_,translation_);

	// �萔�o�b�t�@�ɓ]��
	if (constMap) {
		constMap->matWorld = matWorld_;
	}
}

void WorldTransform::CreateConstBuffer()
{
	const UINT bufferSize = sizeof(ConstBufferDataWorldTransform);
	constBuffer_ = dxCommon_->CreateBufferResource(bufferSize);
}

void WorldTransform::Map()
{
	// �o�b�t�@�̃}�b�s���O
	HRESULT hr = constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
	if (FAILED(hr)) {
		// �G���[�n���h�����O
	}
}

void WorldTransform::UpdateMatrix()
{
	// �X�P�[���A��]�A���s�ړ����������čs����v�Z����
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	// �e������ΐe�̃��[���h�s����|����
	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}

	// �萔�o�b�t�@�ɓ]������
	TransferMatrix();
}

