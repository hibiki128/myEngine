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
	line_->Initialize(SrvManager::GetInstance());
	line_->CreateParticleGroup("line0", "plane.obj");
	vP_.Initialize();
	
	controlPoints_ = {
		{0,  0,  600},
		{100, 100, 610},
		{100, 150, 430},
		{200, 150, 430},
		{200, 0,  600},
		{300, 0,  600},
	};

}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
	ImGui::Begin("camera");
	ImGui::DragFloat3("translate", &vP_.translation_.x, 0.1f);
	ImGui::End();

	//-----シーン切り替え-----
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("GAME");
	}
	//----------------------

	vP_.UpdateMatrix();
	LineDraw();

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
	// 線分の数
	const size_t segmentCount = 100;

	// 始点と終点のリストを作成
	std::vector<Vector3> startPoints;
	std::vector<Vector3> endPoints;

	// 線分の数+1個分の頂点座標を計算
	for (size_t i = 0; i < segmentCount; i++) {
		float t1 = static_cast<float>(i) / segmentCount;
		float t2 = static_cast<float>(i + 1) / segmentCount;

		// Catmull-Rom 補間で始点と終点を計算
		Vector3 startPoint = CatmullRomPosition(controlPoints_, t1);
		Vector3 endPoint = CatmullRomPosition(controlPoints_, t2);

		// 始点と終点をそれぞれのベクターに追加
		startPoints.push_back(startPoint);
		endPoints.push_back(endPoint);
	}

	// LineManager を使って全てのラインを更新
	line_->Update(vP_, startPoints, endPoints);
}
