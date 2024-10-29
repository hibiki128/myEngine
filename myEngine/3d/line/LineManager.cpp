#include "LineManager.h"
#include <random>
#include"TextureManager.h"
#include"fstream"
#include <Quaternion.h>

void LineManager::Initialize(const std::string& filename)
{
	particleCommon = ParticleCommon::GetInstance();
	srvManager_ = SrvManager::GetInstance();
	randomEngine.seed(seedGenerator());

	CreateParticleGroup("line0", filename);
}

#include <cmath> // M_PIを使用するために必要

void LineManager::Update(const ViewProjection& viewProjection, const std::vector<Vector3>& startPoints, const std::vector<Vector3>& endPoints)
{
	assert(startPoints.size() == endPoints.size() && "startPoints and endPoints must have the same number of elements");

	// モデルの間隔を定義
	const float modelPadding = 0.5f;  // モデル同士の間隔を定義

	for (auto& [groupName, lineGroup] : lineGroups) {
		uint32_t numInstance = 0;
		lineGroup.instanceCount = 0;

		// lines のサイズを startPoints に合わせて調整
		auto lineIterator = lineGroup.lines.begin();
		size_t previousSize = lineGroup.lines.size();

		if (previousSize > startPoints.size()) {
			// lines の要素数が startPoints のサイズを超えている場合は削除
			auto toErase = lineGroup.lines.begin();
			std::advance(toErase, startPoints.size());
			lineGroup.lines.erase(toErase, lineGroup.lines.end());
		}
		else {
			// 足りない分を追加
			while (lineGroup.lines.size() < startPoints.size()) {
				Line newLine;
				newLine.transform.Initialize();
				newLine.transform.scale_ = { 0.5f, 0.5f, 0.5f };
				newLine.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);  // デフォルトの色
				lineGroup.lines.push_back(newLine);
			}
		}

		// 各ラインの更新
		lineIterator = lineGroup.lines.begin();  // イテレータをリセット
		for (size_t i = 0; i < startPoints.size(); ++i) {
			const Vector3& startPoint = startPoints[i];
			const Vector3& endPoint = endPoints[i];

			// 中点を求めてtransform.translation_に設定
			Vector3 midPoint = (startPoint + endPoint) * 0.5f;
			lineIterator->transform.translation_ = midPoint;

			// 始点から終点までのベクトルを求め、スケールと回転を設定
			Vector3 lineVector = endPoint - startPoint;
			float length = lineVector.Length();

			// スケールをX方向に設定
			lineIterator->transform.scale_ = { length, 1.0f, 1.0f };  // X方向に線の長さを設定

			// 方向ベクトルを正規化し、Z軸からlineVectorへの回転を計算
			lineVector.Normalize();
			float angle = atan2(lineVector.y, lineVector.x);  // XY平面での回転角度を計算

			// Z軸を基準に回転を設定
			lineIterator->transform.rotation_ = { 0.0f, 0.0f, angle };

			// ワールド行列を作成
			Matrix4x4 worldMatrix = MakeAffineMatrix(
				lineIterator->transform.scale_,
				lineIterator->transform.rotation_,
				lineIterator->transform.translation_
			);

			// ビュープロジェクション行列を計算
			const Matrix4x4& viewProjectionMatrix = viewProjection.matView_ * viewProjection.matProjection_;
			Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;

			// インスタンスデータに設定
			if (numInstance < kNumMaxInstance) {
				instancingData[numInstance].WVP = worldViewProjectionMatrix;
				instancingData[numInstance].World = worldMatrix;
				instancingData[numInstance].color = lineIterator->color;
				++numInstance;
			}

			// instanceCount の更新
			lineGroup.instanceCount++;

			// 次のラインに進む
			++lineIterator;
		}

		// インスタンシングデータのコピー
		if (lineGroup.instancingData) {
			std::memcpy(lineGroup.instancingData, instancingData, sizeof(ParticleForGPU) * numInstance);
		}
	}
}


void LineManager::Draw()
{

	particleCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	for (auto& [groupName, lineGroup] : lineGroups) {
		if (lineGroup.instanceCount > 0) {
			particleCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

			srvManager_->SetGraphicsRootDescriptorTable(1, lineGroup.instancingSRVIndex);

			srvManager_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureIndexByFilePath(lineGroup.material.textureFilePath));

			particleCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), lineGroup.instanceCount, 0, 0);
		}
	}
}

void LineManager::CreateParticleGroup(const std::string name, const std::string& filename)
{
	if (lineGroups.contains(name)) {
		return;
	}

	lineGroups[name] = LineGroup();
	LineGroup& lineGroup = lineGroups[name];
	CreateVartexData(filename);
	lineGroup.material.textureFilePath = modelData.material.textureFilePath;
	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	lineGroup.material.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData.material.textureFilePath);
	lineGroup.instancingResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);

	lineGroup.instancingSRVIndex = srvManager_->Allocate() + 1;
	lineGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&lineGroup.instancingData));

	srvManager_->CreateSRVforStructuredBuffer(lineGroup.instancingSRVIndex, lineGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));

	CreateMaterial();
	lineGroup.instanceCount = 0;
}

void LineManager::CreateVartexData(const std::string& filename)
{
	// インスタンス用のTransformationMatrixリソースを作る
	instancingResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	// 書き込むためのアドレスを取得
	instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	// 単位行列を書き込んでおく
	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
		instancingData[index].WVP = MakeIdentity4x4();
		instancingData[index].World = MakeIdentity4x4();
		instancingData[index].color = { 1.0f,1.0f,1.0f,1.0f };
	}

	modelData = LoadObjFile("resources/models/", filename);

	// 頂点リソースを作る
	vertexResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	// 頂点バッファビューを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();	// リソースの先頭アドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());		// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);	// 1頂点当たりのサイズ

	// 頂点リソースにデータを書き込む
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));		// 書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

LineManager::MaterialData LineManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData; // 構築するMaterialData
	std::string line; // ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // 開けなかったら止める
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	// テクスチャが張られていない場合の処理
	if (materialData.textureFilePath.empty()) {
		materialData.textureFilePath = directoryPath + "/../images/white1x1.png";
	}

	return materialData;
}


LineManager::ModelData LineManager::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::vector<Vector4> positions; // 位置
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行目を格納するもの

	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.x = 1.0f - texcoord.x;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // 区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				VertexData vertex = { position,texcoord };
				modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord };
			}
			// 頂点を逆順で登録することで、周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}


void LineManager::CreateMaterial()
{
	// Sprite用のマテリアルリソースをつくる
	materialResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色の設定
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// Lightingの設定
	materialData->uvTransform = MakeIdentity4x4();
}