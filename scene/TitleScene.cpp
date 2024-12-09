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
	liner_ = DrawLine3D::GetInstance();

	vp_.Initialize();

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("sneakWalk.gltf");
	wt_.Initialize();
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
	DrawSkeleton(obj_.get());
	vp_.UpdateMatrix();
	wt_.UpdateMatrix();
}

void TitleScene::Draw()
{
	/// -------描画処理開始-------
	liner_->Draw(vp_);

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
	
	//-----------------------------


	/// -------描画処理終了-------
}

void TitleScene::DrawSkeleton(Object3d* obj)
{
	// モデルデータを取得
	Model* model = obj->GetModel();

	// スケルトンデータを取得
	const Model::Skeleton& skeleton = model->GetSkeletonData();

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

		// 線の色を設定（デフォルトで白色）
		Vector4 lineColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		// LineManagerに現在の線分を登録
		liner_->SetPoints(parentPosition, childPosition, lineColor);
	}
}

Vector3 TitleScene::ExtractTranslation(const Matrix4x4& matrix)
{
	return Vector3(matrix.m[3][0], matrix.m[3][1], matrix.m[3][2]);
}