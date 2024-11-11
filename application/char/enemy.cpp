#include "enemy.h"
#include"playerBullet.h"

void enemy::Init()
{
	obj3d_ = std::make_unique<Object3d>();
	obj3d_->Initialize("debug/suzannu.obj");
	wt_.Initialize();
	isAlive_ = true;
	HP_ = 5;
	variables_ = GlobalVariables::GetInstance();
	groupName_ = "enemy";
}

void enemy::Update()
{
	if (!itemName.empty()) {
		ApplyVariables();
	}

	if (HP_ <= 0) {
		isAlive_ = false;
	}
	else {
		isAlive_ = true;
	}
	if (isAlive_) {
		Collider::SetCollisionEnabled(true);
	}
	else {
		Collider::SetCollisionEnabled(false);
	}
	wt_.UpdateMatrix();
}

void enemy::Draw(ViewProjection* vp_)
{
	if (isAlive_) {
		obj3d_->Draw(wt_, *vp_);
	}
}

Vector3 enemy::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変更
	Vector3 worldPos = Transformation(offset, wt_.matWorld_);
	return worldPos;
}

Vector3 enemy::GetCenterRotation() const
{
	return wt_.rotation_;
}

AABB enemy::GetAABB() const
{
	// 中心位置を取得
	Vector3 center = GetCenterPosition();
	// スケール値を半分にしてAABBの範囲とする
	Vector3 halfScale = wt_.scale_ * 1.0f;

	// min と max の計算
	AABB aabb;
	aabb.min = center - halfScale;
	aabb.max = center + halfScale;

	return aabb;
}

void enemy::OnCollision(Collider* other)
{
	if (dynamic_cast<playerBullet*>(other)) {
		--HP_;
	}
}

void enemy::imgui()
{
	ImGui::DragFloat3("pos", &wt_.translation_.x, 0.1f);
	ImGui::SliderInt("HP", &HP_, 0, 50);
}

void enemy::AddItem(int enemyNum)
{
	// "enemy" + 数字 + " pos" のように動的に名前を作成する
	itemName = "enemy" + std::to_string(enemyNum) + " pos";

	// 作成した名前と位置情報を追加する
	variables_->AddItem(groupName_, itemName, wt_.translation_);
}

void enemy::ApplyVariables()
{
	wt_.translation_ = variables_->GetVector3Value(groupName_, itemName);
}

