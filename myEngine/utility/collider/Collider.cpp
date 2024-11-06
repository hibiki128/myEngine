#include "Collider.h"
#include"CollisionManager.h"

int Collider::counter = -1;  // 初期値を-1に変更

Collider::Collider() {
	Cubewt_.Initialize();
	AABBwt_.Initialize();
	CollisionManager::AddCollider(this);
	sphere_ = std::make_unique<Object3d>();
	sphere_->Initialize("debug/Collider.obj");
	AABB_ = std::make_unique<Object3d>();
	AABB_->Initialize("debug/AABB.obj");
	variables_ = GlobalVariables::GetInstance();

	// 派生クラス名を取得して整形
	std::string className = typeid(*this).name();
	size_t pos = className.find("Collider");
	if (pos != std::string::npos) {
		className.replace(pos, 8, ""); // "Collider"部分を削除
	}

	counter++;
	groupName = className + " Collider" + std::to_string(counter);

	// 初期化
	SphereOffset = { 0.0f, 0.0f, 0.0f };
	MinOffset = { 0.0f, 0.0f, 0.0f };
	MaxOffset = { 0.0f, 0.0f, 0.0f };

	// グループがまだ存在しない場合のみ作成
	if (!variables_->GroupExists(groupName)) {
		variables_->CreateGroup(groupName);
		variables_->AddItem(groupName, "Cube Translation", SphereOffset);
		variables_->AddItem(groupName, "AABB Min", MinOffset);
		variables_->AddItem(groupName, "AABB Max", MaxOffset);
	}
	color_.SetColor({ 0.0f,0.0f,0.0f,1.0f });
}

Collider::~Collider()
{
	CollisionManager::RemoveCollider(this);
	counter--;  // カウンターをデクリメント
}

void Collider::Initialize() {
}

void Collider::UpdateWorldTransform() {
	ApplyVariables();

	// 球用のワールドトランスフォームを更新
	Cubewt_.translation_ = GetCenterPosition() + SphereOffset;
	Cubewt_.scale_ = { radius_, radius_, radius_ };
	Cubewt_.UpdateMatrix();

	// AABBの現在の最小点と最大点を取得
	aabb = GetAABB();
	aabb.min = aabb.min + MinOffset;
	aabb.max = aabb.max + MaxOffset;

	// AABBの最小点と最大点からスケールと中心位置を計算
	aabbCenter = (aabb.min + aabb.max) * 0.5f;
	aabbScale = (aabb.max - aabb.min) * 0.5f;

	// AABB用ワールドトランスフォームの更新
	AABBwt_.translation_ = aabbCenter;
	AABBwt_.scale_ = aabbScale;
	AABBwt_.UpdateMatrix();
}

void Collider::DrawSphere(const ViewProjection& viewProjection) {
	sphere_->Draw(Cubewt_, viewProjection, &color_, true);
}

void Collider::DrawAABB(const ViewProjection& viewProjection)
{
	AABB_->Draw(AABBwt_, viewProjection, &color_, false);
}

void Collider::ApplyVariables()
{
	SphereOffset = variables_->GetVector3Value(groupName, "Cube Translation");
	MinOffset = variables_->GetVector3Value(groupName, "AABB Min");
	MaxOffset = variables_->GetVector3Value(groupName, "AABB Max");
}