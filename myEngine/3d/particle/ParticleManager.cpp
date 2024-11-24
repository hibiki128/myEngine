#include "ParticleManager.h"
#include <random>
#include"TextureManager.h"
#include"fstream"

void ParticleManager::Initialize(SrvManager* srvManager)
{
	particleCommon = ParticleCommon::GetInstance();
	srvManager_ = srvManager;
	randomEngine.seed(seedGenerator());
}

void ParticleManager::Update(const ViewProjection& viewProjection)
{
	// ビュー射影行列の計算
	Matrix4x4 viewProjectionMatrix = viewProjection.matView_ * viewProjection.matProjection_;

	// カメラの回転成分のみを抽出してビルボード行列を作成
	Matrix4x4 billboardMatrix = viewProjection.matView_;
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
	billboardMatrix.m[3][3] = 1.0f;

	// ビルボード行列を逆行列にすることで、カメラに対して正しい向きにする
	billboardMatrix = Inverse(billboardMatrix);

	for (auto& [groupName, particleGroup] : particleGroups) {

		uint32_t numInstance = 0;

		// 各パーティクルの更新
		for (auto particleIterator = particleGroup.particles.begin();
			particleIterator != particleGroup.particles.end();) {

			// パーティクルの生存時間チェック
			if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
				particleIterator = particleGroup.particles.erase(particleIterator);
				continue;
			}
			float t = (*particleIterator).currentTime / (*particleIterator).lifeTime;
			t = std::clamp(t, 0.0f, 1.0f);
			(*particleIterator).transform.scale_ = (1.0f - t) * (*particleIterator).startScale + t * (*particleIterator).endScale;
			(*particleIterator).Acce = (1.0f - t) * (*particleIterator).startAcce + t * (*particleIterator).endAcce;
			(*particleIterator).transform.rotation_ = (1.0f - t) * (*particleIterator).startRote + t * (*particleIterator).endRote;
			(*particleIterator).velocity += (*particleIterator).Acce;
			// パーティクルの移動
			(*particleIterator).transform.translation_ +=
				(*particleIterator).velocity * kDeltaTime;
			(*particleIterator).currentTime += kDeltaTime;


			// アルファ値の計算
			(*particleIterator).color.w = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);

			// ワールド行列の計算
			Matrix4x4 worldMatrix{};
			if (isBillboard) {
				// ビルボード処理の場合、カメラ方向に向ける
				worldMatrix = MakeScaleMatrix((*particleIterator).transform.scale_) * billboardMatrix *
					MakeTranslateMatrix((*particleIterator).transform.translation_);
			}
			else {
				// 通常のアフィン変換行列を使用
				worldMatrix = MakeAffineMatrix((*particleIterator).transform.scale_,
					(*particleIterator).transform.rotation_,
					(*particleIterator).transform.translation_);
			}


			// パーティクルのワールド位置をチェック
			WorldTransform particleWorldTransform;
			particleWorldTransform.translation_ = (*particleIterator).transform.translation_;

			//// 視野外判定
			//if (viewProjection.IsOutsideViewFrustum(particleWorldTransform)) {
			//	// 視野外であれば、パーティクルを削除
			//	particleIterator = particleGroup.particles.erase(particleIterator);
			//	continue;
			//}

			// ワールド・ビュー・プロジェクション行列の計算
			Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;

			// インスタンスデータに設定
			if (numInstance < kNumMaxInstance) {
				particleGroup.instancingData[numInstance].WVP = worldViewProjectionMatrix;
				particleGroup.instancingData[numInstance].World = worldMatrix;
				particleGroup.instancingData[numInstance].color = (*particleIterator).color;
				particleGroup.instancingData[numInstance].color.w = (*particleIterator).color.w;  // アルファ値の設定
				++numInstance;
			}

			++particleIterator;
		}

		// インスタンス数の更新
		particleGroup.instanceCount = numInstance;
	}
}

void ParticleManager::Draw()
{

	particleCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	for (auto& [groupName, particleGroup] : particleGroups) {
		if (particleGroup.instanceCount > 0) {
			particleCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

			srvManager_->SetGraphicsRootDescriptorTable(1, particleGroup.instancingSRVIndex);

			srvManager_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureIndexByFilePath(particleGroup.material.textureFilePath));

			particleCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), particleGroup.instanceCount, 0, 0);
		}
	}
}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string& filename)
{
	if (particleGroups.contains(name)) {
		return;
	}

	particleGroups[name] = ParticleGroup();
	ParticleGroup& particleGroup = particleGroups[name];
	CreateVartexData(filename);
	particleGroup.material.textureFilePath = modelData.material.textureFilePath;
	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	particleGroup.instancingResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);

	particleGroup.instancingSRVIndex = srvManager_->Allocate() + 1;
	particleGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instancingData));

	srvManager_->CreateSRVforStructuredBuffer(particleGroup.instancingSRVIndex, particleGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));

	CreateMaterial();
	particleGroup.instanceCount = 0;
}

void ParticleManager::CreateVartexData(const std::string& filename)
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

ParticleManager::Particle ParticleManager::MakeNewParticle(
	std::mt19937& randomEngine,
	const Vector3& translate,
	const Vector3& scale, // スケールを引数として受け取る
	const Vector3& velocityMin, const Vector3& velocityMax, // 速度の範囲
	float lifeTimeMin, float lifeTimeMax,
	const Vector3& particleStartScale, const Vector3& particleEndScale,
	const Vector3& startAcce, const Vector3& endAcce,
	const Vector3& startRote, const Vector3& endRote,
	bool isRamdomColor, float alphaMin, float alphaMax) // サイズの開始値と終了値をVector3で受け取る
{
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distVelocityX(velocityMin.x, velocityMax.x);
	std::uniform_real_distribution<float> distVelocityY(velocityMin.y, velocityMax.y);
	std::uniform_real_distribution<float> distVelocityZ(velocityMin.z, velocityMax.z);
	std::uniform_real_distribution<float> distLifeTime(lifeTimeMin, lifeTimeMax);
	std::uniform_real_distribution<float> distAlpha(alphaMin, alphaMax);

	Particle particle;

	// スケールを考慮したランダムな位置を計算
	Vector3 randomTranslate = {
		distribution(randomEngine) * scale.x,
		distribution(randomEngine) * scale.y,
		distribution(randomEngine) * scale.z
	};
	particle.transform.translation_ = translate + randomTranslate;

	particle.startScale = particleStartScale;
	particle.endScale = particleEndScale;
	particle.startAcce = startAcce;
	particle.endAcce = endAcce;
	particle.startRote = startRote;
	particle.endRote = endRote;

	// 速度をランダムに設定
	particle.velocity = {
		distVelocityX(randomEngine),
		distVelocityY(randomEngine),
		distVelocityZ(randomEngine)
	};


	if (isRamdomColor) {
		// ランダムな色を設定
		std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
		particle.color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), distAlpha(randomEngine) };
	}
	else {
		particle.color = { 1.0f,1.0f,1.0f, distAlpha(randomEngine) };
	}

	// ライフタイムをランダムに設定
	particle.lifeTime = distLifeTime(randomEngine);
	particle.currentTime = 0.0f;

	return std::move(particle);
}

ParticleManager::MaterialData ParticleManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
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


ParticleManager::ModelData ParticleManager::LoadObjFile(const std::string& directoryPath, const std::string& filename)
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


void ParticleManager::CreateMaterial()
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

std::list<ParticleManager::Particle> ParticleManager::Emit(
	const std::string name,
	const Vector3& position,
	uint32_t count,
	const Vector3& scale, // スケールを引数として追加
	const Vector3& velocityMin, const Vector3& velocityMax, // 速度の範囲を引数として追加
	float lifeTimeMin, float lifeTimeMax,
	const Vector3& particleStartScale, const Vector3& particleEndScale,
	const Vector3& startAcce, const Vector3& endAcce,
	const Vector3& startRote, const Vector3& endRote,
	bool isRandomColor,  float alphaMin, float alphaMax) // サイズの開始値と終了値を引数として追加
{
	// パーティクルグループが存在するか確認
	assert(particleGroups.find(name) != particleGroups.end() && "Error: パーティクルグループが存在しません。");

	// 指定されたパーティクルグループを取得
	ParticleGroup& particleGroup = particleGroups[name];

	// 新しいパーティクルを生成し、パーティクルグループに追加
	std::list<Particle> newParticles;
	for (uint32_t nowCount = 0; nowCount < count; ++nowCount) {
		// 新たなパーティクルを作成
		Particle particle = MakeNewParticle(
			randomEngine,
			position,
			scale,          // 追加されたスケール
			velocityMin,
			velocityMax,    // 追加された速度の範囲
			lifeTimeMin,
			lifeTimeMax,    // 追加されたライフタイムの範囲
			particleStartScale,
			particleEndScale,
			startAcce,
			endAcce,
			startRote,
			endRote,
			isRandomColor,
			alphaMin,
			alphaMax
		);
		newParticles.push_back(particle);
	}

	// 新たに生成されたパーティクルをグループに追加
	particleGroup.particles.splice(particleGroup.particles.end(), newParticles);

	return newParticles; // 作成されたパーティクルを返す
}
