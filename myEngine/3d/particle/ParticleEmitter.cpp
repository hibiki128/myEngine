#include "ParticleEmitter.h"

// コンストラクタ
ParticleEmitter::ParticleEmitter()
	:
	velocityMin_{ -1.0f, -1.0f, -1.0f }, // 速度の最小値
	velocityMax_{ 1.0f, 1.0f, 1.0f }, // 速度の最大値
	lifeTimeMin_{ 1.0f }, // ライフタイムの最小値
	lifeTimeMax_{ 3.0f }, // ライフタイムの最大値
	isVisible{ true }      // 初期状態は可視

{}

void ParticleEmitter::Initialize(const std::string& name, const std::string& fileName)
{
	emitterObj = std::make_unique<Object3d>();
	emitterObj->Initialize("debug/OBB.obj");
	name_ = name;
	transform_.Initialize();
	Manager_ = std::make_unique<ParticleManager>();
	Manager_->Initialize(SrvManager::GetInstance());
	Manager_->CreateParticleGroup(name_, fileName);
	emitFrequency_ = 0.1f;
	//transform_.UpdateMatrix();
	startAcce_ = { 1.0f,1.0f,1.0f };
	endAcce_ = { 1.0f,1.0f,1.0f };
	startScale_ = { 1.0f,1.0f,1.0f };
	endScale_ = { 1.0f,1.0f,1.0f };
	alphaMin_ = 1.0f;
	alphaMax_ = 1.0f;
	AddItem();
	isBillBoard = false;
	isActive_ = true;
	ApplyGlobalVariables();
}

// Update関数
void ParticleEmitter::Update(const ViewProjection& vp_) {
	SetValue();
	// 経過時間を進める
	elapsedTime_ += deltaTime;

	// 発生頻度に基づいてパーティクルを発生させる
	while (elapsedTime_ >= emitFrequency_) {
		Emit();  // パーティクルを発生させる
		elapsedTime_ -= emitFrequency_;  // 過剰に進んだ時間を考慮
	}
	Manager_->Update(vp_);
	transform_.UpdateMatrix();
}

void ParticleEmitter::UpdateOnce(const ViewProjection& vp_)
{
	SetValue();
	if (!isActive_) {
		Emit();  // パーティクルを発生させる
		isActive_ = true;
	}
	Manager_->Update(vp_);
	transform_.UpdateMatrix();
}

void ParticleEmitter::Draw()
{
	Manager_->Draw();
}

void ParticleEmitter::DrawEmitter(const ViewProjection& vp_)
{
	// isVisibleがtrueのときだけ描画
	if (isVisible) {
		emitterObj->Draw(transform_, vp_, nullptr, false);
	}
}

// Emit関数
void ParticleEmitter::Emit() {
	// ParticleManagerのEmit関数を呼び出す
	Manager_->Emit(
		name_,
		transform_.translation_,
		count_,
		transform_.scale_,          // スケールを引数として渡す
		velocityMin_,               // 最小速度を引数として渡す
		velocityMax_,               // 最大速度を引数として渡す
		lifeTimeMin_,               // 最小ライフタイムを引数として渡す
		lifeTimeMax_,               // 最大ライフタイムを引数として渡す
		startScale_,
		endScale_,
		startAcce_,
		endAcce_,
		startRote_,
		endRote_,
		isRandomColor,
		alphaMin_,
		alphaMax_
	);
}

void ParticleEmitter::ApplyGlobalVariables()
{
	emitFrequency_ = globalVariables->GetFloatValue(groupName, "emitFrequency");
	count_ = globalVariables->GetIntValue(groupName, "count");
	//transform_.translation_ = globalVariables->GetVector3Value(groupName, "Emit translation");
	transform_.scale_ = globalVariables->GetVector3Value(groupName, "Emit scale");
	startScale_ = globalVariables->GetVector3Value(groupName, "Particle StartScale");
	endScale_ = globalVariables->GetVector3Value(groupName, "Particle EndScale");
	startRote_ = globalVariables->GetVector3Value(groupName, "Particle StartRote");
	endRote_ = globalVariables->GetVector3Value(groupName, "Particle EndRote");
	startAcce_ = globalVariables->GetVector3Value(groupName, "Particle StartAcce");
	endAcce_ = globalVariables->GetVector3Value(groupName, "Particle EndAcce");
	velocityMin_ = globalVariables->GetVector3Value(groupName, "minVelocity");
	velocityMax_ = globalVariables->GetVector3Value(groupName, "maxVelocity");
	lifeTimeMax_ = globalVariables->GetFloatValue(groupName, "lifeTimeMax");
	lifeTimeMin_ = globalVariables->GetFloatValue(groupName, "lifeTimeMin");
	isVisible = globalVariables->GetBoolValue(groupName, "isVisible");
	isBillBoard = globalVariables->GetBoolValue(groupName, "isBillBoard");
	isRandomColor = globalVariables->GetBoolValue(groupName, "isRamdomColor");
	alphaMin_ = globalVariables->GetFloatValue(groupName, "alphaMin");
	alphaMax_ = globalVariables->GetFloatValue(groupName, "alphaMax");
}

void ParticleEmitter::SetValue()
{

	globalVariables->SetValue(groupName, "emitFrequency", emitFrequency_);
	globalVariables->SetValue(groupName, "count", count_);
	globalVariables->SetValue(groupName, "Emit scale", transform_.scale_);
	globalVariables->SetValue(groupName, "Particle StartScale", startScale_);
	globalVariables->SetValue(groupName, "Particle StartRote", startRote_);
	globalVariables->SetValue(groupName, "Particle EndRote", endRote_);
	globalVariables->SetValue(groupName, "Particle EndScale", endScale_);
	globalVariables->SetValue(groupName, "Particle StartAcce", startAcce_);
	globalVariables->SetValue(groupName, "Particle EndAcce", endAcce_);
	globalVariables->SetValue(groupName, "minVelocity", velocityMin_);
	globalVariables->SetValue(groupName, "maxVelocity", velocityMax_);
	globalVariables->SetValue(groupName, "lifeTimeMax", lifeTimeMax_);
	globalVariables->SetValue(groupName, "lifeTimeMin", lifeTimeMin_);
	globalVariables->SetValue(groupName, "isVisible", isVisible);
	globalVariables->SetValue(groupName, "isBillBoard", isBillBoard);
	globalVariables->SetValue(groupName, "isRamdomColor", isRandomColor);
	globalVariables->SetValue(groupName, "alphaMin", alphaMin_);
	globalVariables->SetValue(groupName, "alphaMax", alphaMax_);
}

void ParticleEmitter::AddItem()
{
	groupName = name_.c_str();
	globalVariables = GlobalVariables::GetInstance();
	globalVariables->CreateGroup(groupName);

	globalVariables->AddItem(groupName, "emitFrequency", emitFrequency_);
	globalVariables->AddItem(groupName, "count", count_);
	//globalVariables->AddItem(groupName, "Emit translation", transform_.translation_);
	globalVariables->AddItem(groupName, "Emit scale", transform_.scale_);
	globalVariables->AddItem(groupName, "Particle StartScale", startScale_);
	globalVariables->AddItem(groupName, "Particle EndScale", endScale_);
	globalVariables->AddItem(groupName, "Particle StartRote", startRote_);
	globalVariables->AddItem(groupName, "Particle EndRote", endRote_);
	globalVariables->AddItem(groupName, "Particle StartAcce", startAcce_);
	globalVariables->AddItem(groupName, "Particle EndAcce", endAcce_);
	globalVariables->AddItem(groupName, "minVelocity", velocityMin_);
	globalVariables->AddItem(groupName, "maxVelocity", velocityMax_);
	globalVariables->AddItem(groupName, "lifeTimeMax", lifeTimeMax_);
	globalVariables->AddItem(groupName, "lifeTimeMin", lifeTimeMin_);
	globalVariables->AddItem(groupName, "isVisible", isVisible);
	globalVariables->AddItem(groupName, "isBillBoard", isBillBoard);
	globalVariables->AddItem(groupName, "isRamdomColor", isRandomColor);
	globalVariables->AddItem(groupName, "alphaMin", alphaMin_);
	globalVariables->AddItem(groupName, "alphaMax", alphaMax_);
}

// ImGuiで値を動かす関数
void ParticleEmitter::RenderImGui() {
#ifdef _DEBUG
	ImGui::Begin(name_.c_str());

	// 基本データセクション
	if (ImGui::CollapsingHeader("Emitter Data")) {
		// トランスフォームデータをフレーム内に配置
		ImGui::Text("Transform Data:");
		ImGui::Separator();
		ImGui::Columns(2, "TransformColumns", false); // 2列レイアウト
		ImGui::Text("Position"); ImGui::NextColumn();
		ImGui::DragFloat3("##Position", &transform_.translation_.x, 0.1f);
		ImGui::NextColumn();
		ImGui::Text("Rotation"); ImGui::NextColumn();
		ImGui::DragFloat3("##Rotation", &transform_.rotation_.x, 0.1f);
		ImGui::NextColumn();
		ImGui::Text("Scale"); ImGui::NextColumn();
		ImGui::DragFloat3("##Scale", &transform_.scale_.x, 0.1f);
		ImGui::Columns(1); // 列終了
		ImGui::Separator();

		// 可視性フラグ
		ImGui::Checkbox("Visible", &isVisible);
	}

	// パーティクルデータセクション
	if (ImGui::CollapsingHeader("Particle Data")) {
		// LifeTimeを折りたたみ可能にする
		if (ImGui::TreeNode("Lifetime")) {
			ImGui::Text("Lifetime Settings:");
			ImGui::Separator();
			ImGui::DragFloat("LifeTime Min", &lifeTimeMin_, 0.1f, 0.0f, 10.0f);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Particle minimum lifetime in seconds.");
			}
			ImGui::DragFloat("LifeTime Max", &lifeTimeMax_, 0.1f, 0.0f, 10.0f);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Particle maximum lifetime in seconds.");
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		// 速度と加速度
		if (ImGui::TreeNode("Velocity and Acceleration")) {
			ImGui::Text("Velocity:");
			ImGui::DragFloat3("Velocity Min", &velocityMin_.x, 0.1f);
			ImGui::DragFloat3("Velocity Max", &velocityMax_.x, 0.1f);

			ImGui::Text("Acceleration:");
			ImGui::DragFloat3("Start Acce", &startAcce_.x, 0.01f);
			ImGui::DragFloat3("End Acce", &endAcce_.x, 0.01f);
			ImGui::TreePop();
		}

		ImGui::Separator();

		// サイズ
		if (ImGui::TreeNode("Size")) {
			ImGui::Text("Scale:");
			ImGui::DragFloat3("Start Scale", &startScale_.x, 0.1f);
			ImGui::DragFloat3("End Scale", &endScale_.x, 0.1f);
			ImGui::TreePop();
		}

		ImGui::Separator();

		// 回転
		if (ImGui::TreeNode("Rotation")) {
			ImGui::SliderAngle("Start Rotation X", &startRote_.x);
			ImGui::SliderAngle("End Rotation X", &endRote_.x);
			ImGui::SliderAngle("Start Rotation Y", &startRote_.y);
			ImGui::SliderAngle("End Rotation Y", &endRote_.y);
			ImGui::SliderAngle("Start Rotation Z", &startRote_.z);
			ImGui::SliderAngle("End Rotation Z", &endRote_.z);
			ImGui::TreePop();
		}

		ImGui::Separator();

		
		/// Todo : 透明度をいじっても変更されないので直す
		// Alphaを折りたたみ可能にする
		if (ImGui::TreeNode("Alpha")) {
			ImGui::Text("Alpha Settings:");
			ImGui::DragFloat("Alpha Min", &alphaMin_, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Alpha Max", &alphaMax_, 0.1f, 0.0f, 1.0f);
			ImGui::TreePop();
		}
	}

	// エミット設定セクション
	if (ImGui::CollapsingHeader("Emit Settings")) {
		ImGui::DragFloat("Emit Frequency", &emitFrequency_, 0.1f, 0.1f, 10.0f);
		ImGui::InputInt("Count", &count_, 1, 100);
		count_ = std::clamp(count_, 0, 10000);
	}

	// その他の設定セクション
	if (ImGui::CollapsingHeader("State Settings")) {
		ImGui::Checkbox("Billboard", &isBillBoard);
		ImGui::Checkbox("Random Color", &isRandomColor);
	}

	ImGui::End();
#endif
}
