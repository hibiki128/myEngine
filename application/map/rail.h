#pragma once
#include <vector>
#include <Vector3.h>
#include"line/LineManager.h"
#include"GlobalVariables.h"
#include"myMath.h"
#include"Object3d.h"
class rail
{
public:
	void Initialize(ViewProjection* vp);

	void Update();

	void railDraw();

	void IcoDraw();

	std::vector<Vector3> GetControlPoints() { return controlPoints_; }
private:
	void LineUpdate();

	void ControlPointsSetting();

	void ApplyVariables();
private:
	std::vector<Vector3>controlPoints_; // 制御点
	std::unique_ptr<LineManager> line_; // 線描画クラス


	int segmentCount = 0;               // 分割数 
	
	std::vector<Vector3> startPoints_;                // 始点
	std::vector<Vector3> endPoints_;                  // 終点

	ViewProjection* vp_;

	std::vector<std::unique_ptr<Object3d>> segments_;
	std::vector<WorldTransform> wts_;

	GlobalVariables* variables = nullptr;
	std::string groupName;

};

