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
	emitterObj->Initialize("OBB.obj");
	name_ = name;
	transform_.Initialize();
	emitFrequency_ = 1.0f;
	count_ = 1;
	Manager_ = std::make_unique<ParticleManager>();
	Manager_->Initialize(SrvManager::GetInstance());
	Manager_->CreateParticleGroup(name_, fileName);
	transform_.UpdateMatrix();
}

// Update関数
void ParticleEmitter::Update(const ViewProjection& vp_) {
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
		lifeTimeMax_               // 最大ライフタイムを引数として渡す
	);
}

// ImGuiで値を動かす関数
void ParticleEmitter::RenderImGui() {
#ifdef _DEBUG


	if (ImGui::BeginTabBar("emitter"))
	{
		if (ImGui::BeginTabItem(name_.c_str()))
		{
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