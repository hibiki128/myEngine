#define NOMINMAX
#include "Collider.h"
#include"CollisionManager.h"
#include <line/DrawLine3D.h>

int Collider::counter = -1;  // 初期値を-1に変更

Collider::Collider() {
	Cubewt_.Initialize();
	AABBwt_.Initialize();
	OBBwt_.Initialize();
	CollisionManager::AddCollider(this);
	variables_ = GlobalVariables::GetInstance();

	// 派生クラス名を取得して整形
	std::string className = typeid(*this).name();
	size_t pos = className.find("Collider");
	if (pos != std::string::npos) {
		className.replace(pos, 8, ""); // "Collider"部分を削除
	}

	counter++;
	groupName = className + " Collider" + std::to_string(counter);

	// 初期化
	SphereOffset = { 0.0f, 0.0f, 0.0f };
	AABBOffset.min = { 0.0f, 0.0f, 0.0f };
	AABBOffset.max = { 0.0f, 0.0f, 0.0f };
	OBBOffset.rotationCenter = { 0.0f,0.0f,0.0f };
	OBBOffset.scaleCenter = { 0.0f,0.0f,0.0f };
	OBBOffset.size = { 1.0f,1.0f,1.0f };

	// グループがまだ存在しない場合のみ作成
	if (!variables_->GroupExists(groupName)) {
		variables_->CreateGroup(groupName);
		variables_->AddItem(groupName, "Sphere Translation", SphereOffset);
		variables_->AddItem(groupName, "AABB Min", AABBOffset.min);
		variables_->AddItem(groupName, "AABB Max", AABBOffset.max);
		variables_->AddItem(groupName, "OBB rotateCenter", OBBOffset.rotationCenter);
		variables_->AddItem(groupName, "OBB scaleCenter", OBBOffset.scaleCenter);
		variables_->AddItem(groupName, "OBB size", OBBOffset.size);
	}
}

Collider::~Collider()
{
	CollisionManager::RemoveCollider(this);
	counter--;  // カウンターをデクリメント
}

void Collider::Initialize() {
}

void Collider::UpdateWorldTransform() {
	ApplyVariables();

	// 球用のワールドトランスフォームを更新
	Cubewt_.translation_ = GetCenterPosition() + SphereOffset;
	Cubewt_.scale_ = { radius_, radius_, radius_ };
	Cubewt_.UpdateMatrix();

	// AABBの現在の最小点と最大点を取得
	aabb.min = GetCenterPosition() - scale_;
	aabb.max = GetCenterPosition() + scale_;
	aabb.min = aabb.min + AABBOffset.min;
	aabb.max = aabb.max + AABBOffset.max;

	// AABBの最小点と最大点からスケールと中心位置を計算
	aabbCenter = (aabb.min + aabb.max) * 0.5f;
	aabbScale = (aabb.max - aabb.min) * 0.5f;

	// AABB用ワールドトランスフォームの更新
	AABBwt_.translation_ = aabbCenter;
	AABBwt_.scale_ = aabbScale;
	AABBwt_.UpdateMatrix();

	// OBBの各プロパティを更新
	obb.rotationCenter = GetCenterPosition() + OBBOffset.rotationCenter; // 回転中心
	obb.scaleCenter = GetCenterPosition() + OBBOffset.scaleCenter;       // スケール中心

	// OBBの向きベクトルを計算
	MakeOBBOrientations(obb, GetCenterRotation());

	// サイズを更新
	obb.size = OBBOffset.size;

	// ワールドトランスフォームの更新
	OBBwt_.translation_ = obb.scaleCenter;
	OBBwt_.rotation_ = GetCenterRotation();
	OBBwt_.scale_ = obb.size;
	OBBwt_.UpdateMatrix();
}

void Collider::DrawSphere(const ViewProjection& viewProjection) {
	const uint32_t kSubdivision = 10;                                        // 分割数
	const float kLonEvery = 2.0f * std::numbers::pi_v<float> / kSubdivision; // 経度分割1つ分の角度
	const float kLatEvery = std::numbers::pi_v<float> / kSubdivision;        // 緯度分割1つ分の角度

	// 緯度の方向に分割　-π/2 ～ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex; // 現在の緯度

		// 経度の方向に分割 0 ～ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; // 現在の経度

			// 現在の点を求める
			Vector3 start = {
				Cubewt_.translation_.x + Cubewt_.scale_.x * std::cosf(lat) * std::cosf(lon),
				Cubewt_.translation_.y + Cubewt_.scale_.y * std::sinf(lat),
				Cubewt_.translation_.z + Cubewt_.scale_.z * std::cosf(lat) * std::sinf(lon)
			};

			// 次の点を求める（経度方向）
			Vector3 end1 = {
				Cubewt_.translation_.x + Cubewt_.scale_.x * std::cosf(lat) * std::cosf(lon + kLonEvery),
				Cubewt_.translation_.y + Cubewt_.scale_.y * std::sinf(lat),
				Cubewt_.translation_.z + Cubewt_.scale_.z * std::cosf(lat) * std::sinf(lon + kLonEvery),
			};

			// 次の点を求める（緯度方向）
			Vector3 end2 = {
				Cubewt_.translation_.x + Cubewt_.scale_.x * std::cosf(lat + kLatEvery) * std::cosf(lon),
				Cubewt_.translation_.y + Cubewt_.scale_.y * std::sinf(lat + kLatEvery),
				Cubewt_.translation_.z + Cubewt_.scale_.z * std::cosf(lat + kLatEvery) * std::sinf(lon),
			};

			// 線を描画（経度方向）
			DrawLine3D::GetInstance()->SetPoints(start, end1, color_);
			// 線を描画（緯度方向）
			DrawLine3D::GetInstance()->SetPoints(start, end2, color_);
		}
	}
}


void Collider::DrawAABB(const ViewProjection& viewProjection)
{
	// AABBの頂点リスト
	std::array<Vector3, 8> vertices = {
		aabb.min,
		{ aabb.max.x, aabb.min.y, aabb.min.z },
		{ aabb.min.x, aabb.max.y, aabb.min.z },
		{ aabb.max.x, aabb.max.y, aabb.min.z },
		{ aabb.min.x, aabb.min.y, aabb.max.z },
		{ aabb.max.x, aabb.min.y, aabb.max.z },
		{ aabb.min.x, aabb.max.y, aabb.max.z },
		{ aabb.max.x, aabb.max.y, aabb.max.z }
	};

	// エッジ接続リスト
	const std::array<std::pair<int, int>, 12> edges = {
		std::make_pair(0, 1), std::make_pair(1, 3), std::make_pair(3, 2), std::make_pair(2, 0), // 前面
		std::make_pair(4, 5), std::make_pair(5, 7), std::make_pair(7, 6), std::make_pair(6, 4), // 背面
		std::make_pair(0, 4), std::make_pair(1, 5), std::make_pair(2, 6), std::make_pair(3, 7)  // 側面
	};

	// 線を描画
	for (const auto& edge : edges) {
		DrawLine3D::GetInstance()->SetPoints(vertices[edge.first], vertices[edge.second], color_);
	}
}

void Collider::DrawOBB(const ViewProjection& viewProjection) {
	// OBBの8つの頂点を計算
	std::array<Vector3, 8> vertices;
	Vector3 halfSize = obb.size; // サイズの半分を計算

	// OBBの8頂点を計算するループ
	for (int i = 0; i < 8; i++) {
		// 各頂点のローカル座標を計算
		Vector3 localPosition = Vector3(
			(i & 1) ? halfSize.x : -halfSize.x,
			(i & 2) ? halfSize.y : -halfSize.y,
			(i & 4) ? halfSize.z : -halfSize.z
		);

		// scaleCenter を基準にスケール変換
		Vector3 scaledPosition = localPosition + (obb.scaleCenter - obb.rotationCenter);

		// 回転中心を基準に回転を適用
		Vector3 rotatedPosition =
			obb.orientations[0] * scaledPosition.x +
			obb.orientations[1] * scaledPosition.y +
			obb.orientations[2] * scaledPosition.z;

		// ワールド座標へ変換
		vertices[i] = obb.rotationCenter + rotatedPosition;
	}

	// 回転後にscaleCenterの位置を計算
	obb.scaleCenterRotated = obb.orientations[0] * (obb.scaleCenter.x - obb.rotationCenter.x) +
		obb.orientations[1] * (obb.scaleCenter.y - obb.rotationCenter.y) +
		obb.orientations[2] * (obb.scaleCenter.z - obb.rotationCenter.z) + obb.rotationCenter;

	// scaleCenterに球を描画
	DrawSphereAtCenter(viewProjection, obb.scaleCenterRotated, 0.1f);  // 半径0.1fで球を描画

	// エッジ接続リスト
	const std::array<std::pair<int, int>, 12> edges = {
		std::make_pair(0, 1), std::make_pair(1, 3), std::make_pair(3, 2), std::make_pair(2, 0), // 前面
		std::make_pair(4, 5), std::make_pair(5, 7), std::make_pair(7, 6), std::make_pair(6, 4), // 背面
		std::make_pair(0, 4), std::make_pair(1, 5), std::make_pair(2, 6), std::make_pair(3, 7)  // 側面
	};

	// 線を描画
	for (const auto& edge : edges) {
		//DrawLine3D::GetInstance()->SetPoints(vertices[edge.first], vertices[edge.second], color_);
	}

	DrawRotationCenter(viewProjection);
}

// 球を描画する関数
void Collider::DrawSphereAtCenter(const ViewProjection& viewProjection, const Vector3& center, float radius) {
	const int slices = 16;  // 球の横方向の分割数
	const int stacks = 8;   // 球の縦方向の分割数

	for (int i = 0; i < slices; i++) {
		float theta1 = (i * 2.0f * std::numbers::pi_v<float>) / slices;
		float theta2 = ((i + 1) * 2.0f * std::numbers::pi_v<float>) / slices;

		for (int j = 0; j < stacks; j++) {
			float phi1 = (j * std::numbers::pi_v<float>) / stacks;
			float phi2 = ((j + 1) * std::numbers::pi_v<float>) / stacks;

			// 球の4つの頂点を計算
			Vector3 p1 = Vector3(
				center.x + radius * sin(phi1) * cos(theta1),
				center.y + radius * cos(phi1),
				center.z + radius * sin(phi1) * sin(theta1)
			);

			Vector3 p2 = Vector3(
				center.x + radius * sin(phi1) * cos(theta2),
				center.y + radius * cos(phi1),
				center.z + radius * sin(phi1) * sin(theta2)
			);

			Vector3 p3 = Vector3(
				center.x + radius * sin(phi2) * cos(theta1),
				center.y + radius * cos(phi2),
				center.z + radius * sin(phi2) * sin(theta1)
			);

			Vector3 p4 = Vector3(
				center.x + radius * sin(phi2) * cos(theta2),
				center.y + radius * cos(phi2),
				center.z + radius * sin(phi2) * sin(theta2)
			);

			// 4つの三角形で球の断片を描画
			//DrawLine3D::GetInstance()->SetPoints(p1, p2);
			//DrawLine3D::GetInstance()->SetPoints(p2, p4);
			//DrawLine3D::GetInstance()->SetPoints(p4, p3);
			//DrawLine3D::GetInstance()->SetPoints(p3, p1);
		}
	}
}


void Collider::DrawRotationCenter(const ViewProjection& viewProjection) {
	// 回転中心を表す球の半径
	float rotationCenterRadius = 0.1f;

	// 球を構成する点を描画
	const uint32_t kSubdivision = 10; // 分割数
	const float kLonEvery = 2.0f * std::numbers::pi_v<float> / kSubdivision;
	const float kLatEvery = std::numbers::pi_v<float> / kSubdivision;

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			Vector3 start = {
				obb.rotationCenter.x + rotationCenterRadius * std::cosf(lat) * std::cosf(lon),
				obb.rotationCenter.y + rotationCenterRadius * std::sinf(lat),
				obb.rotationCenter.z + rotationCenterRadius * std::cosf(lat) * std::sinf(lon)
			};

			Vector3 end1 = {
				obb.rotationCenter.x + rotationCenterRadius * std::cosf(lat) * std::cosf(lon + kLonEvery),
				obb.rotationCenter.y + rotationCenterRadius * std::sinf(lat),
				obb.rotationCenter.z + rotationCenterRadius * std::cosf(lat) * std::sinf(lon + kLonEvery),
			};

			Vector3 end2 = {
				obb.rotationCenter.x + rotationCenterRadius * std::cosf(lat + kLatEvery) * std::cosf(lon),
				obb.rotationCenter.y + rotationCenterRadius * std::sinf(lat + kLatEvery),
				obb.rotationCenter.z + rotationCenterRadius * std::cosf(lat + kLatEvery) * std::sinf(lon),
			};

			//DrawLine3D::GetInstance()->SetPoints(start, end1);
			//DrawLine3D::GetInstance()->SetPoints(start, end2);
		}
	}
}

void Collider::ApplyVariables()
{
	SphereOffset = variables_->GetVector3Value(groupName, "Sphere Translation");
	AABBOffset.min = variables_->GetVector3Value(groupName, "AABB Min");
	AABBOffset.max = variables_->GetVector3Value(groupName, "AABB Max");
	OBBOffset.rotationCenter = variables_->GetVector3Value(groupName, "OBB rotateCenter");
	OBBOffset.scaleCenter = variables_->GetVector3Value(groupName, "OBB scaleCenter");
	OBBOffset.size = variables_->GetVector3Value(groupName, "OBB size");
}

void Collider::MakeOBBOrientations(OBB& obb, const Vector3& rotate) {
	// 回転行列を作成
	Matrix4x4 rotateMatrix = MakeRotateXMatrix(rotate.x) * MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z);

	// 各方向ベクトルを計算
	obb.orientations[0].x = rotateMatrix.m[0][0];
	obb.orientations[0].y = rotateMatrix.m[0][1];
	obb.orientations[0].z = rotateMatrix.m[0][2];

	obb.orientations[1].x = rotateMatrix.m[1][0];
	obb.orientations[1].y = rotateMatrix.m[1][1];
	obb.orientations[1].z = rotateMatrix.m[1][2];

	obb.orientations[2].x = rotateMatrix.m[2][0];
	obb.orientations[2].y = rotateMatrix.m[2][1];
	obb.orientations[2].z = rotateMatrix.m[2][2];
}
