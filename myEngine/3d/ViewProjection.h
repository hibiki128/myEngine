#pragma once
#include"Matrix4x4.h"
#include"Vector3.h"
#include "wrl.h"
#include"numbers"
#include"myMath.h"
#include "d3d12.h"
#include"DirectXCommon.h"

struct ConstBufferDataViewProjection {
	Matrix4x4 view;
	Matrix4x4 projection;
	Vector3 cameraPos;
};

class ViewProjection
{
public:
	// ���[�J����]�p
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };
	// ���[�J�����W
	Vector3 translation_ = { 0.0f,0.0f,-50.0f };

	// ������������p
	float fovAngleY = 45.0f * std::numbers::pi_v<float>;
	// �r���[�|�[�g�̃A�X�y�N�g��
	float aspectRatio = (float)16 / 9;
	// �[�x���E(��O��)
	float nearZ = 0.1f;
	// �[�x���E(����)
	float farZ = 1000.0f;

	// �r���[�s��
	Matrix4x4 matView_;
	// �ˉe�s��
	Matrix4x4 matProjection_;

	ViewProjection() = default;
	~ViewProjection() = default;

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �萔�o�b�t�@����
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// �}�b�s���O����
	/// </summary>
	void Map();

	/// <summary>
	/// �s����X�V����
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// �s���]������
	/// </summary>
	void TransferMatrix();

	/// <summary>
	/// �r���[�s����X�V����
	/// </summary>
	void UpdateViewMatrix();

	/// <summary>
	/// �ˉe�s����X�V����
	/// </summary>
	void UpdateProjectionMatrix();

	/// <summary>
	/// �萔�o�b�t�@�̎擾
	/// </summary>
	/// <returns>�萔�o�b�t�@</returns>
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }
private:

	DirectXCommon* dxCommon_ = nullptr;

	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
	// �}�b�s���O�ς݃A�h���X
	ConstBufferDataViewProjection* constMap = nullptr;
	// �R�s�[�֎~
	ViewProjection(const ViewProjection&) = delete;
	ViewProjection& operator=(const ViewProjection&) = delete;

};

static_assert(!std::is_copy_assignable_v<ViewProjection>);
