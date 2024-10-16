#include "Enemy.h"
#include "application/CollisionManager.h"

void Enemy::Initialize()
{
    obj_ = std::make_unique<Object3d>();
    obj_->Initialize("Sphere.obj");

    worldTransform_.Initialize();

    hp_ = maxHp_;
    isAlive_ = true;

    boundingBox_ = BoundingBox::Sphere_3D;
    radius_ = 0.5f;

    Collider::SetAtrribute("enemy");
    Collider::SetMask("enemy");

}

void Enemy::Update()
{
    if (ImGui::BeginTabBar("enemy"))
    {
        if (ImGui::BeginTabItem("Enemy"))
        {
            ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
            ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
            ImGui::DragFloat3("positoin", &worldTransform_.translation_.x, 0.01f);
            ImGui::InputInt("maxHP", reinterpret_cast<int*>(&maxHp_));
            ImGui::Text("hp: %d", hp_);

            if (ImGui::Button("HP_Reset"))
                hp_ = maxHp_;

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    if (isAlive_)
        CollisionManager::GetInstance()->SetCollider(this);

}

void Enemy::Draw(const ViewProjection& _vp)
{
    if(isAlive_)
        obj_->Draw(worldTransform_, _vp);
}

Vector3 Enemy::GetWorldPosition()
{
    Vector3 result;
    result.x = worldTransform_.matWorld_.m[3][0];
    result.y = worldTransform_.matWorld_.m[3][1];
    result.z = worldTransform_.matWorld_.m[3][2];
    return result;
}

void Enemy::OnCollision()
{
    hp_--;
    if (hp_ <= 0)
        isAlive_ = false;
}
