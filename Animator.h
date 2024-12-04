#pragma once
#include"assimp/Importer.hpp"
#include"assimp/scene.h"
#include"assimp/postprocess.h"
#include <Vector3.h>
#include <Quaternion.h>
#include <vector>
#include <map>
#include <string>
#include <Matrix4x4.h>

class Model;
class Animator
{
private:
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

	std::string directoryPath = "resources/models/";

private:
	Animation animation;
	float animationTime = 0.0f;
	Model* model_ = nullptr;
	Matrix4x4 localMatrix;
	bool isNotAnimation = false;
private:

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
public:
	void Initialize(Model* model, const std::string& filename);

	void Update();

	Animation LoadAnimationFile(const std::string& filename);

	Matrix4x4 GetLocalMatrix() { return localMatrix; }

	bool IsNotAnimation() { return isNotAnimation; }
};
