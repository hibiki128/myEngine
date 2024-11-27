#include "Model.h"
#include "myMath.h"
#include"fstream"
#include"sstream"
#include "TextureManager.h"

void Model::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename)
{
	// 引数で受け取ってメンバ変数に記録する
	modelCommon_ = modelCommon;

	// モデル読み込み
	modelData = LoadObjFile(directorypath, filename);

	CreateVartexData();

	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);

	// 単位行列を書き込んでおく
	modelData.material.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData.material.textureFilePath);
}

void Model::Draw()
{

	modelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	srvManager_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData.material.textureFilePath));
	// 描画！（DrawCall/ドローコール）
	modelCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}

void Model::CreateVartexData()
{
	// Sprite用の頂点リソースを作る
	vertexResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点データの設定
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
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


Model::ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
    ModelData modelData;
    std::vector<Vector4> positions; // 位置
    std::vector<Vector3> normals;   // 法線
    std::vector<Vector2> texcoords; // テクスチャ座標
    std::string line;

    // ファイル名からフォルダ部分を取得
    std::string folderPath;
    size_t lastSlashPos = filename.find_last_of("/\\");
    if (lastSlashPos != std::string::npos) {
        folderPath = filename.substr(0, lastSlashPos);
    }

    // ファイルを開く
    std::ifstream file(directoryPath + filename);
    assert(file.is_open()); // 開けなければ停止

    // 頂点数の予測：メモリ確保を効率化するための推定
    positions.reserve(10000);
    normals.reserve(10000);
    texcoords.reserve(10000);

    // ファイルを1行ずつ読み取る
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; // コメント行をスキップ

        std::istringstream s(line);
        std::string identifier;
        s >> identifier;

        if (identifier == "v") { // 頂点位置
            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.x *= -1.0f; // X座標の反転
            position.w = 1.0f;
            positions.push_back(position);
        }
        else if (identifier == "vt") { // テクスチャ座標
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoord.y = 1.0f - texcoord.y; // Y座標の反転
            texcoords.push_back(texcoord);
        }
        else if (identifier == "vn") { // 法線
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normal.x *= -1.0f; // X座標の反転
            normals.push_back(normal);
        }
        else if (identifier == "f") { // 面データ
            VertexData triangle[3];
            for (int32_t i = 0; i < 3; ++i) {
                std::string vertexDefinition;
                s >> vertexDefinition;
                uint32_t posIdx = 0, texIdx = 0, normIdx = 0;
                sscanf_s(vertexDefinition.c_str(), "%u/%u/%u", &posIdx, &texIdx, &normIdx);
                VertexData vertex = {
                    positions[posIdx - 1],
                    texcoords[texIdx - 1],
                    normals[normIdx - 1]
                };
                modelData.vertices.push_back(vertex);
                triangle[i] = vertex;
            }
        }
        else if (identifier == "mtllib") { // マテリアル
            std::string materialFilename;
            s >> materialFilename;
            if (!folderPath.empty()) {
                modelData.material = LoadMaterialTemplateFile(directoryPath + folderPath, materialFilename);
            }
            else {
                modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
            }
        }
    }

    return modelData;
}
