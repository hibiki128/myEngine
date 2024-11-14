#include "rail.h"

void rail::Initialize(ViewProjection* vp)
{
	vp_ = vp;
	segmentCount =250;
	startPoints_.resize(segmentCount);
	endPoints_.resize(segmentCount);

	// 初期制御点を4つ生成
	controlPoints_.emplace_back(Vector3(-5.0f, 0.0f, 0.0f)); // 制御点1
	controlPoints_.emplace_back(Vector3(-2.5f, 0.0f, 0.0f)); // 制御点2
	controlPoints_.emplace_back(Vector3(2.5f, 0.0f, 0.0f)); // 制御点3
	controlPoints_.emplace_back(Vector3(5.0f, 0.0f, 0.0f));  // 制御点4

	line_ = std::make_unique<LineManager>();
	line_->Initialize("debug/plane.obj");

	// 制御点の数に応じてセグメントを初期化
	segments_.resize(controlPoints_.size());
	wts_.resize(controlPoints_.size());

	for (size_t i = 0; i < controlPoints_.size(); ++i) {
		segments_[i] = std::make_unique<Object3d>(); // オブジェクトを生成
		segments_[i]->Initialize("debug/ICO.obj"); // ICOモデルを初期化
		wts_[i].translation_ = controlPoints_[i]; // ワールドトランスフォームに制御点の座標を設定
		wts_[i].UpdateMatrix(); // 行列を更新
	}

	groupName = "rail";
	variables = GlobalVariables::GetInstance();
	variables->CreateGroup(groupName);

	// 初期制御点と制御点数をJSONに保存
	for (size_t i = 0; i < controlPoints_.size(); ++i) {
		variables->AddItem(groupName, "controlPoint" + std::to_string(i), controlPoints_[i]);
	}
	variables->AddItem(groupName, "ControlPointCount", static_cast<int>(controlPoints_.size()));

	ApplyVariables();
}

void rail::Update()
{
	LineUpdate();

	// 各ワールドトランスフォームの行列を更新
	for (size_t i = 0; i < wts_.size(); ++i) {
		wts_[i].UpdateMatrix(); // 各ワールドトランスフォームの行列を更新
	}
}

void rail::railDraw()
{
	line_->Draw();
}

void rail::IcoDraw()
{
	// 制御点を描画
	for (size_t i = 0; i < segments_.size(); ++i) {
		segments_[i]->Draw(wts_[i], *vp_); // オブジェクトを描画
	}
}

void rail::imgui()
{
	if (ImGui::BeginTabBar("Line")) {

		if (ImGui::BeginTabItem("Line")) {
			ControlPointsSetting();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void rail::LineUpdate()
{
	// 現在のセグメントの始点と終点を保存するための変数
	Vector3 prevPos;

	for (int i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;  // 0.0fから1.0fまでの間でtを計算
		Vector3 pos = CatmullRomPosition(controlPoints_, t); // カタムル・ロム曲線に基づく位置を計算

		// セグメントの始点と終点を設定
		if (i > 0) {
			startPoints_[i - 1] = prevPos; // 前の位置を始点として追加
			endPoints_[i - 1] = pos;       // 現在の位置を終点として追加
		}

		// 前の位置を更新
		prevPos = pos;
	}

	// 線の更新
	line_->Update(*vp_, startPoints_, endPoints_);
}

void rail::ControlPointsSetting()
{
	// 制御点の数を表示
	ImGui::Text("Control Points Count: %d", controlPoints_.size());

	// 制御点の追加ボタン
	if (ImGui::Button("Add Control Point")) {
		controlPoints_.emplace_back(Vector3(0.0f, 0.0f, 0.0f));

		// 新しいセグメントを初期化
		segments_.emplace_back(std::make_unique<Object3d>());
		segments_.back()->Initialize("debug/ICO.obj");
		wts_.emplace_back(WorldTransform());

		// 新しい制御点をJSONに保存
		variables->AddItem(groupName, "controlPoint" + std::to_string(controlPoints_.size() - 1), controlPoints_.back());

		// 制御点数をJSONに保存（更新）
		variables->SetValue(groupName, "ControlPointCount", static_cast<int>(controlPoints_.size()));
	}

	// 各制御点の位置を表示し、調整可能にする
	for (size_t i = 0; i < controlPoints_.size(); ++i) {
		ImGui::PushID(int(i));

		ImGui::DragFloat3("Control Point", &controlPoints_[i].x, 0.1f);
		wts_[i].translation_ = controlPoints_[i];
		wts_[i].scale_ = { 0.3f,0.3f,0.3f };
		wts_[i].UpdateMatrix();

		// JSONの値も更新
		variables->SetValue(groupName, "controlPoint" + std::to_string(i), controlPoints_[i]);
		ImGui::PopID();
	}
}

void rail::ApplyVariables()
{
	// JSONから制御点の数を取得
	int controlPointCount = variables->GetIntValue(groupName, "ControlPointCount");

	// 制御点リスト、セグメントリスト、ワールドトランスフォームリストを取得した数で再設定
	controlPoints_.resize(controlPointCount);
	segments_.resize(controlPointCount);
	wts_.resize(controlPointCount);

	for (int i = 0; i < controlPointCount; ++i) {
		// JSONから各制御点の座標を取得
		controlPoints_[i] = variables->GetVector3Value(groupName, "controlPoint" + std::to_string(i));

		// 新しいセグメントを初期化
		segments_[i] = std::make_unique<Object3d>();
		segments_[i]->Initialize("debug/ICO.obj");

		// ワールドトランスフォームの設定
		wts_[i].translation_ = controlPoints_[i];
		wts_[i].UpdateMatrix();
	}
}



