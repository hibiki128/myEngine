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
	AddItem();
	isBillBoard = false;
	isActive_ = false;
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
	isBillBoard = false;
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
		endRote_
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
}

// ImGuiで値を動かす関数
void ParticleEmitter::RenderImGui() {
#ifdef _DEBUG

	if (ImGui::BeginTabBar(name_.c_str())) {
		if (ImGui::BeginTabItem(name_.c_str())) {
			Manager_->imgui();
			// transform_.translation_の表示と編集
			ImGui::DragFloat3("Position", &transform_.translation_.x, 0.1f); // x, y, z

			// transform_.rotation_の表示と編集
			ImGui::DragFloat3("Rotation", &transform_.rotation_.x, 0.1f); // x, y, z

			// transform_.scale_の表示と編集
			ImGui::DragFloat3("Scale", &transform_.scale_.x, 0.1f); // x, y, z

			// emitFrequency_の表示と編集
			ImGui::DragFloat("Emit Frequency", &emitFrequency_, 0.1f, 0.1f, 10.0f); // 0.1〜5.0の範囲

			// countの表示と編集
			ImGui::InputInt("Count", &count_, 1, 10000);

			// 0から10000の範囲に制限する
			count_ = std::clamp(count_, 0, 10000);

			// 速度の最小値と最大値の表示と編集
			ImGui::DragFloat3("Velocity Min", &velocityMin_.x, 0.1f);
			ImGui::DragFloat3("Velocity Max", &velocityMax_.x, 0.1f);

			// ライフタイムの最小値と最大値の表示と編集
			ImGui::DragFloat("LifeTime Min", &lifeTimeMin_, 0.1f);
			ImGui::DragFloat("LifeTime Max", &lifeTimeMax_, 0.1f);

			ImGui::DragFloat3("StartScale", &startScale_.x, 0.1f);
			ImGui::DragFloat3("EndScale", &endScale_.x, 0.1f);

	ImGui::DragFloat3("StartAcce", &startAcce_.x, 0.01f);
	ImGui::DragFloat3("EndAcce", &endAcce_.x, 0.01f);

			ImGui::SliderAngle("StartRoteX", &startRote_.x, 0.1f);
			ImGui::SliderAngle("StartRoteY", &startRote_.y, 0.1f);
			ImGui::SliderAngle("StartRoteZ", &startRote_.z, 0.1f);
			ImGui::SliderAngle("EndRoteX", &endRote_.x, 0.1f);
			ImGui::SliderAngle("EndRoteY", &endRote_.y, 0.1f);
			ImGui::SliderAngle("EndRoteZ", &endRote_.z, 0.1f);

			// isVisibleフラグの表示と編集
			ImGui::Checkbox("Visible", &isVisible); // 可視性のチェックボックス

			ImGui::Checkbox("BillBoard", &isBillBoard);
			Manager_->SetBillBorad(isBillBoard);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

#endif // _DEBUG

}