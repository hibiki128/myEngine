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

void LineManager::Update(const ViewProjection& viewProjection, const std::vector<Vector3>& startPoints, const std::vector<Vector3>& endPoints)
{
	// startPoints と endPoints のサイズが一致するか確認
	assert(startPoints.size() == endPoints.size() && "startPoints and endPoints must have the same number of elements");

	for (auto& [groupName, lineGroup] : lineGroups) {
		uint32_t numInstance = 0;

		// まずは instanceCount をリセット
		lineGroup.instanceCount = 0;

		// lines のサイズを startPoints のサイズに合わせて調整
		size_t previousSize = lineGroup.lines.size(); // 前のサイズを保存
		if (previousSize > startPoints.size()) {
			// lines の要素数が startPoints のサイズを上回る場合、余分な要素を削除
			lineGroup.lines.resize(startPoints.size());
		}
		else {
			// lines のサイズを startPoints のサイズに合わせて追加
			while (lineGroup.lines.size() < startPoints.size()) {
				Line newLine; // Line クラスのインスタンスを生成
				newLine.transform.Initialize();
				newLine.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // デフォルトの色を設定（必要に応じて変更）
				lineGroup.lines.push_back(newLine);
			}
		}

		// 各ラインの更新
		auto lineIterator = lineGroup.lines.begin();
		for (size_t i = 0; i < startPoints.size(); ++i) {
			if (lineIterator == lineGroup.lines.end()) {
				// lines の要素が不足している場合、処理を終了
				break;
			}

			const Vector3& startPoint = startPoints[i];
			const Vector3& endPoint = endPoints[i];

			// 中点の計算（始点と終点の中間点を求める）
			Vector3 midPoint = (startPoint + endPoint) * 0.5f;
			lineIterator->transform.translation_ = midPoint;

			// スケールの計算（始点と終点の距離を求めてスケールに反映）
			Vector3 lineVector = endPoint - startPoint;
			float length = lineVector.Length(); // 2点間の距離
			lineIterator->transform.scale_ = Vector3(length, 0.002f, 0.002f); // X方向に線の長さを反映、他の軸は0.2

			// クォータニオンで回転の計算（始点から終点への回転を計算）
			Quaternion rotationQuat;
			rotationQuat.SetFromTo(Vector3(1.0f, 0.0f, 0.0f), lineVector.Normalize()); // X軸方向から目的の方向への回転を計算
			Vector3 eulerRotation = rotationQuat.ToEulerAngles(); // オイラー角に変換して設定
			lineIterator->transform.rotation_ = eulerRotation;

			// アフィン行列（ワールド行列）を作成
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
				lineGroup.instancingData[numInstance].WVP = worldViewProjectionMatrix;
				lineGroup.instancingData[numInstance].World = worldMatrix;
				lineGroup.instancingData[numInstance].color = lineIterator->color; // colorも更新
				++numInstance;
			}

			// ライン処理が終わったら instanceCount を+1
			lineGroup.instanceCount++;

			// 次のライン要素に進む
			++lineIterator;
		}
	}
}



void LineManager::Draw()
{

	//Update(viewProjection,startPoints,endPoints);

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
	lineGroup.instancingResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);

	lineGroup.instancingSRVIndex = srvManager_->Allocate() + 1;
	lineGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&lineGroup.instancingData));

	srvManager_->CreateSRVforStructuredBuffer(lineGroup.instancingSRVIndex, lineGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));

	CreateMaterial();
	lineGroup.instanceCount = 0;
}

void LineManager::CreateVartexData(const std::string& filename)
{
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
		materialData.textureFilePath = directoryPath + "/" + "white1x1.png";
	}

	return materialData;
}


LineManager::ModelData LineManager::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::vector<Vector4> positions; // 位置
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行目を格納するもの

	// ファイル名からフォルダ部分を取得
	std::string folderPath;
	size_t lastSlashPos = filename.find_last_of("/\\");
	if (lastSlashPos != std::string::npos) {
		// ファイル名の前にフォルダがある場合は、そのフォルダ部分を使用する
		folderPath = filename.substr(0, lastSlashPos);
	}

	std::ifstream file(directoryPath + filename); // ファイルを開く
	assert(file.is_open()); // ファイルが開けなかったら停止

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読み込む

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
		/*	texcoord.x = 1.0f - texcoord.x;*/
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
				VertexData vertex = { position, texcoord };
				modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position, texcoord };
			}
		}
		else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;

			if (!folderPath.empty()) {
				// ファイル名の前にフォルダがあればそれを追加する
				modelData.material = LoadMaterialTemplateFile(directoryPath + folderPath, materialFilename);
			}
			else {
				// ファイル名の前にフォルダがあればそれを追加する
				modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
			}
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