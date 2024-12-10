#pragma once
#include"ModelCommon.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector2.h"
#include"SrvManager.h"
#include"assimp/Importer.hpp"
#include"assimp/scene.h"
#include"assimp/postprocess.h"
#include "map"
#include"Quaternion.h"
#include "span"
#include"array"
#include <PipeLineManager.h>

class Model
{
public:
	struct QuaternionTransform {
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
	};

	// 頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct MaterialData
	{
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};

	struct Node {
		QuaternionTransform transform;
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node>children;
	};

	struct Joint {
		QuaternionTransform transform;
		Matrix4x4 localMatrix;
		Matrix4x4 skeletonSpaceMatrix;
		std::string name;
		std::vector<int32_t> children;
		int32_t index;
		std::optional<int32_t>parent;
	};

	struct Skeleton {
		int32_t root;
		std::map<std::string, int32_t>jointMap;
		std::vector<Joint>joints;
	};

	struct KeyframeVector3 {
		Vector3 value;
		float time;
	};

	struct KeyframeQuaternion {
		Quaternion value;
		float time;
	};

	struct NodeAnimation {
		std::vector<KeyframeVector3> translate;
		std::vector<KeyframeQuaternion> rotate;
		std::vector<KeyframeVector3> scale;
	};

	struct Animation {
		float duration;
		std::map<std::string, NodeAnimation>nodeAnimations;
	};

	struct VertexWeightData {
		float weight;
		uint32_t vertexIndex;
	};

	struct JointWeightData {
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData>vertexWeights;
	};

	struct ModelData
	{
		std::map<std::string, JointWeightData>skinClusterData;
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		MaterialData material;
		Node rootNode;
	};

	static const uint32_t kNumMaxInfluence = 4;
	struct VertexInfluence {
		std::array<float, kNumMaxInfluence> weights;
		std::array<int32_t, kNumMaxInfluence> jointIndices;
	};

	struct WellForGPU {
		Matrix4x4 skeletonSpaceMatrix;
		Matrix4x4 skeletonSpaceInverseTransposeMatrix;
	};

	struct SkinCluster {
		std::vector<Matrix4x4>inverseBindPoseMatrices;
		Microsoft::WRL::ComPtr<ID3D12Resource>influenceResource;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<VertexInfluence>mappedInfluence;
		Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
		std::span<WellForGPU> mappedPalette;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>paletteSrvHandle;
	};
	uint32_t skinClusterSrvIndex_ = 0;
private:

	ModelCommon* modelCommon_;

	// Objファイルのデータ
	ModelData modelData;
	SrvManager* srvManager_;

	std::string filename_;
	std::string directorypath_;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	uint32_t* indexData;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	bool isGltf = false;

	Animation animation_;
	Skeleton skeleton_;
	SkinCluster skinCluster_;
	float animationTime = 0.0f;
	Model* model_ = nullptr;
	Matrix4x4 localMatrix;
	bool haveAnimation = false;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelCommon"></param>
	void Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void SkinClusterUpdate(SkinCluster& skinCluster,const Skeleton& skeleton);

	ModelData GetModelData() { return modelData; }
	Skeleton GetSkeletonData() { return skeleton_; }
	void SetSrv(SrvManager* srvManager) { srvManager_ = srvManager; }

	bool IsGltf() { return isGltf; }

	/// <summary>
	/// 骨の更新
	/// </summary>
	void SkeletonUpdate(Skeleton& skeleton);
private:

	/// <summary>
	/// 頂点データ作成
	/// </summary>
	void CreateVartexData();

	/// <summary>
	/// indexの作成
	/// </summary>
	void CreateIndexResource();

	/// <summary>
	/// .mtlファイルの読み取り
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns></returns>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	///  .objファイルの読み取り
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns></returns>
	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// ノード読み取り
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	Node ReadNode(aiNode* node);

	/// <summary>
	/// アニメーションファイル読み込み
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns></returns>
	Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// 値の計算(Vector3)
	/// </summary>
	/// <param name="keyframes"></param>
	/// <param name="time"></param>
	/// <returns></returns>
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	/// <summary>
	/// 値の計算(Quaternion)
	/// </summary>
	/// <param name="keyframes"></param>
	/// <param name="time"></param>
	/// <returns></returns>
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	/// <summary>
	/// 骨作成
	/// </summary>
	/// <param name="rootNode"></param>
	/// <returns></returns>
	Skeleton CreateSkeleton(const Node& rootNode);

	/// <summary>
	/// Joint作成
	/// </summary>
	/// <param name="node"></param>
	/// <param name="parent"></param>
	/// <param name="joints"></param>
	/// <returns></returns>
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	/// <summary>
	/// アニメーションの適応
	/// </summary>
	/// <param name="skeleton"></param>
	/// <param name="animation"></param>
	/// <param name="animtionTime"></param>
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animtionTime);

	/// <summary>
	/// SkinClusterの生成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="skeleton"></param>
	/// <param name="modelData"></param>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <returns></returns>
	SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData);
};

