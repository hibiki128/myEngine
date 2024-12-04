#include "Animator.h"
#include <cassert>
#include"Model.h"
#include <myMath.h>

void Animator::Initialize(Model* model, const std::string& filename)
{
	model_ = model;

	isNotAnimation = false;

	LoadAnimationFile(filename);
}

void Animator::Update()
{
	animationTime += 1.0f / 60.0f;
	animationTime = std::fmod(animationTime, animation.duration);
	NodeAnimation& rootNodeAnimation = animation.nodeAnimations[model_->GetModelData().rootNode.name];
	Vector3 translate = CalculateValue(rootNodeAnimation.translate, animationTime);
	Quaternion rotate = CalculateValue(rootNodeAnimation.rotate, animationTime);
	Vector3 scale = CalculateValue(rootNodeAnimation.scale, animationTime);
	localMatrix = MakeAffineMatrix(scale, rotate.ToEulerAngles(), translate);
}

Animator::Animation Animator::LoadAnimationFile(const std::string& filename)
{
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	if (!scene || scene->mNumAnimations == 0) {
		// アニメーションがない場合は何もせず、空のアニメーションを返す
		isNotAnimation = true;
		return animation;
	}
	aiAnimation* animationAssimp = scene->mAnimations[0]; // 最初のanimationだけ採用。もちろん複数対応するに越したことはない
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // 時間の単位を秒に変換
	// assimpではここのNodeのAnimationをchannelと呼んでいるのでchannelを回してNodeAnimationの情報を取ってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];

		// Position
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z }; // 右手->左手(座標変換)
			nodeAnimation.translate.push_back(keyframe);
		}

		// Scale
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // 秒に変換
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 右手->左手変換は不要
			nodeAnimation.scale.push_back(keyframe);
		}

		// Rotation
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // 秒に変換

			// 右手->左手座標系の変換
			keyframe.value = {
				keyAssimp.mValue.x,
				 -keyAssimp.mValue.y,
				 -keyAssimp.mValue.z,
				keyAssimp.mValue.w
			};
			nodeAnimation.rotate.push_back(keyframe);
		}

	}
	return animation;
}

Vector3 Animator::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time)
{
	assert(!keyframes.empty()); // キーがないものは返す値がわからないのでダメ
	if (keyframes.size() == 1 || time <= keyframes[0].time) { // キーが一つまたは時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのキーフレームを取得して範囲内に時刻があるか確認
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を保管する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまできた場合は1番後の時刻よりも後ろなので最後の値を返す
	return (*keyframes.rbegin()).value;
}

Quaternion Animator::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time)
{
	assert(!keyframes.empty()); // キーフレームが空でないことを確認
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		// キーフレームが一つしかないか、時刻が最初のキーフレームより前なら最初の値を返す
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのキーフレームを取得して範囲内に時刻があるか確認
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 時刻が範囲内の場合は補間を行う
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			Quaternion q;
			q = q.Sleap(keyframes[index].value, keyframes[nextIndex].value, t);
			return q;
		}
	}

	// ここまで来た場合は最後の時刻よりも後ろなので最後の値を返す
	return (*keyframes.rbegin()).value;
}