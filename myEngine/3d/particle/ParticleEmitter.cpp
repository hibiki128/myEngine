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
	count_ = 3;
	alphaMin_ = 1.0f;
	alphaMax_ = 1.0f;
	AddItem();
	isBillBoard = false;
	isActive_ = true;
	isAcceMultiply = false;
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
	Manager_->SetRandomRotate(isRandomRotate);
	Manager_->SetAcceMultipy(isAcceMultiply);
	Manager_->SetBillBorad(isBillBoard);
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
	isRandomRotate = globalVariables->GetBoolValue(groupName, "isRandomRotate");
	isAcceMultiply = globalVariables->GetBoolValue(groupName, "isAcceMultiply"); 
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
	globalVariables->SetValue(groupName, "isRandomRotate", isRandomRotate);
	globalVariables->SetValue(groupName, "isAcceMultiply", isAcceMultiply);
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
	globalVariables->AddItem(groupName, "isRandomRotate", isRandomRotate);
	globalVariables->AddItem(groupName, "isAcceMultiply", isAcceMultiply);
}

// ImGuiで値を動かす関数
void ParticleEmitter::RenderImGui() {
#ifdef _DEBUG
	ImGui::Begin(name_.c_str());

	// 基本データセクション
	if (ImGui::CollapsingHeader("エミッターデータ")) {
		// トランスフォームデータをフレーム内に配置
		ImGui::Text("Transformデータ:");
		ImGui::Separator();
		ImGui::Columns(2, "TransformColumns", false); // 2列レイアウト
		ImGui::Text("位置"); ImGui::NextColumn();
		ImGui::DragFloat3("##位置", &transform_.translation_.x, 0.1f);
		ImGui::NextColumn();
		ImGui::Text("回転"); ImGui::NextColumn();
		ImGui::DragFloat3("##回転", &transform_.rotation_.x, 0.1f);
		ImGui::NextColumn();
		ImGui::Text("大きさ"); ImGui::NextColumn();
		ImGui::DragFloat3("##大きさ", &transform_.scale_.x, 0.1f);
		ImGui::Columns(1); // 列終了
		ImGui::Separator();

		// 可視性フラグ
		ImGui::Checkbox("表示", &isVisible);
	}

	// パーティクルデータセクション
	if (ImGui::CollapsingHeader("パーティクルデータ")) {
		// LifeTimeを折りたたみ可能にする
		if (ImGui::TreeNode("寿命")) {
			ImGui::Text("寿命設定:");
			ImGui::Separator();
			ImGui::DragFloat("最小値", &lifeTimeMin_, 0.1f, 0.0f, 10.0f);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("パーティクルの寿命の最小値です");
			}
			ImGui::DragFloat("最大値", &lifeTimeMax_, 0.1f, 0.0f, 10.0f);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("パーティクルの寿命の最大値です");
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		// 速度と加速度
		if (ImGui::TreeNode("速度、加速度")) {
			ImGui::Text("速度:");
			ImGui::DragFloat3("最小値", &velocityMin_.x, 0.1f);
			ImGui::DragFloat3("最大値", &velocityMax_.x, 0.1f);

			ImGui::Text("加速度:");
			ImGui::DragFloat3("最初", &startAcce_.x, 0.001f);
			ImGui::DragFloat3("最後", &endAcce_.x, 0.001f);
			ImGui::Checkbox("乗算", &isAcceMultiply);
			ImGui::TreePop();
		}

		ImGui::Separator();

		// サイズ
		if (ImGui::TreeNode("大きさ")) {
			ImGui::Text("大きさ:");
			ImGui::DragFloat3("最初", &startScale_.x, 0.1f);
			ImGui::DragFloat3("最後", &endScale_.x, 0.1f);
			ImGui::TreePop();
		}

		ImGui::Separator();

		// 回転
		if (ImGui::TreeNode("回転")) {
			if (!isRandomRotate) {
				ImGui::SliderAngle("最初 X", &startRote_.x);
				ImGui::SliderAngle("最後 X", &endRote_.x);
				ImGui::SliderAngle("最初 Y", &startRote_.y);
				ImGui::SliderAngle("最後 Y", &endRote_.y);
				ImGui::SliderAngle("最初 Z", &startRote_.z);
				ImGui::SliderAngle("最後 Z", &endRote_.z);
			}
			ImGui::Checkbox("ランダムな回転", &isRandomRotate);
			ImGui::TreePop();
		}

		ImGui::Separator();


		/// Todo : 透明度をいじっても変更されないので直す
		// Alphaを折りたたみ可能にする
		if (ImGui::TreeNode("透明度")) {
			ImGui::Text("透明度の設定:");
			ImGui::DragFloat("最小値", &alphaMin_, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("最大値", &alphaMax_, 0.1f, 0.0f, 1.0f);
			ImGui::TreePop();
		}
	}

	// エミット設定セクション
	if (ImGui::CollapsingHeader("パーティクルの数、間隔")) {
		ImGui::DragFloat("間隔", &emitFrequency_, 0.1f, 0.1f, 10.0f);
		ImGui::InputInt("数", &count_, 1, 100);
		count_ = std::clamp(count_, 0, 10000);
	}

	// その他の設定セクション
	if (ImGui::CollapsingHeader("各状態の設定")) {
		ImGui::Checkbox("ビルボード", &isBillBoard);
		ImGui::Checkbox("ランダムカラー", &isRandomColor);
	}

	ImGui::End();
#endif
}
