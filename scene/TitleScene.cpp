#include "TitleScene.h"
#include "ImGuiManager.h"
#include"SceneManager.h"
#include"SrvManager.h"
#include"Model.h"

void TitleScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();

	vp_.Initialize();

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("sneakWalk.gltf");
	wt_.Initialize();

	line_ = std::make_unique<LineManager>();
	line_->Initialize("debug/line.obj");
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("TitleScene");
	if (ImGui::BeginTabBar("camera")) {
		if (ImGui::BeginTabItem("camera")) {
			ImGui::DragFloat3("translation", &vp_.translation_.x, 0.1f);
			ImGui::SliderAngle("X", &vp_.rotation_.x);
			ImGui::SliderAngle("Y", &vp_.rotation_.y);
			ImGui::SliderAngle("Z", &vp_.rotation_.z);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	if (ImGui::BeginTabBar("transform")) {
		if (ImGui::BeginTabItem("transform")) {
			ImGui::DragFloat3("translation", &wt_.translation_.x, 0.1f);
			ImGui::SliderAngle("X", &wt_.rotation_.x);
			ImGui::SliderAngle("Y", &wt_.rotation_.y);
			ImGui::SliderAngle("Z", &wt_.rotation_.z);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
#endif // _DEBUG

	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("GAME");
	}
	//----------------------
	DrawSkeleton(obj_.get(),50);
	vp_.UpdateMatrix();
	wt_.UpdateMatrix();
}

void TitleScene::Draw()
{
	/// -------描画処理開始-------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----
	
	//------------------------

	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----
	obj_->Draw(wt_, vp_);
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	line_->Draw();
	//-----------------------------

	/// -------描画処理終了-------
}

void TitleScene::DrawSkeleton(Object3d* obj, int segmentCount)
{
	// モデルデータを取得
	Model* model = obj->GetModel();

	// スケルトンデータを取得
	const Model::Skeleton& skeleton = model->GetSkeletonData();

	// 描画用の開始点と終了点リスト
	std::vector<Vector3> startPoints;
	std::vector<Vector3> endPoints;

	// 各ジョイントを巡回して親子関係の線を生成
	for (const auto& joint : skeleton.joints) {
		// 親がいない場合、このジョイントはルートなのでスキップ
		if (!joint.parent.has_value()) {
			continue;
		}

		// 親ジョイントを取得
		const auto& parentJoint = skeleton.joints[*joint.parent];

		// 親と子のスケルトン空間座標を取得
		Vector3 parentPosition = ExtractTranslation(parentJoint.skeltonSpaceMatrix);
		Vector3 childPosition = ExtractTranslation(joint.skeltonSpaceMatrix);

		// 線を分割して中間点を計算
		Vector3 step = (childPosition - parentPosition) / static_cast<float>(segmentCount);
		Vector3 currentPoint = parentPosition;

		for (int i = 0; i < segmentCount; ++i) {
			Vector3 nextPoint = currentPoint + step;

			// 開始点と終了点をリストに追加
			startPoints.push_back(currentPoint);
			endPoints.push_back(nextPoint);

			// 次の開始点を更新
			currentPoint = nextPoint;
		}
	}

	// LineManagerに更新を適用
	line_->Update(vp_, startPoints, endPoints);
}


Vector3 TitleScene::ExtractTranslation(const Matrix4x4& matrix)
{
	return Vector3(matrix.m[3][0], matrix.m[3][1], matrix.m[3][2]);
}