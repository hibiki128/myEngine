#include "TitleScene.h"
#include "ImGuiManager.h"
#include"SceneManager.h"
#include"SrvManager.h"

void TitleScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();

	line_ = std::make_unique<LineManager>();
	line_->Initialize("plane.obj");
	vP_.Initialize();
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
	ImGui::Begin("camera");
	ImGui::DragFloat3("translate", &vP_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &vP_.rotation_.x, 0.1f);
	ImGui::End();

	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("GAME");
	}
	//----------------------

	LineDraw();
	vP_.UpdateMatrix();

}

void TitleScene::Draw()
{
	/// -------描画処理開始-------


	/// 3Dオブジェクトの描画準備
	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----

	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------
	line_->Draw();
	//-----------------------------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----

	//------------------------



	/// ----------------------------------
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Draw();
#endif // _DEBUG
	/// -------描画処理終了-------
}

void TitleScene::LineDraw()
{
	// ImGuiウィンドウの開始
	ImGui::Begin("Control Points");

	// 「Add Control Point」ボタンを作成
	if (ImGui::Button("Add Control Point")) {
		// 最後の制御点の位置を基に新たな制御点を追加
		Vector3 lastPoint = controlPoints.back();
		controlPoints.push_back(Vector3(lastPoint.x + 1.0f, lastPoint.y + 1.0f, lastPoint.z));
	}

	// 各制御点の位置を編集可能にする
	for (size_t i = 0; i < controlPoints.size(); ++i) {
		std::string label = "Control Point " + std::to_string(i);
		ImGui::DragFloat3(label.c_str(), &controlPoints[i].x, 0.1f);
	}
	ImGui::End();

	// 線分の数（制御点の数 - 1）を計算
	size_t segmentCount = controlPoints.size() - 1;

	// 始点と終点のリストをクリア
	std::vector<Vector3> startPoints;
	std::vector<Vector3> endPoints;

	// 各制御点ペアに基づいて始点と終点を設定
	for (size_t i = 0; i < segmentCount; ++i) {
		Vector3 startPoint = controlPoints[i];
		Vector3 endPoint = controlPoints[i + 1];

		startPoints.push_back(startPoint);
		endPoints.push_back(endPoint);
	}

	// LineManagerを使って全てのラインを更新
	line_->Update(vP_, startPoints, endPoints);
}