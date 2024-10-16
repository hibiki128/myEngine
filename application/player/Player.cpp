#include "Player.h"
#include "Input.h"
#include "ModelManager.h"
#include "application/Screw.h"
#include "application/CollisionManager.h"

Player::~Player()
{
    for (PlayerBullet* bullet : bullets_)
    {
        delete bullet;
        bullet = nullptr;
    }
    bullets_.clear();
}

void Player::Initialize()
{
    input_ = Input::GetInstance();
    obj_ = std::make_unique<Object3d>();
    obj_->Initialize("Player.obj");

    worldTransform_.Initialize();

    position_ = 0.0f;
    velocity_ = 0.0f;
    minVelocity_ = -100.0f;
    downAcceleration_ = 3.0f;
    baseAscendSpeed_ = 5.5f;
    multiplier = 1.0f / 10000.0f;

    maxChargeTime_ = 120u;
    remainingAmmo_ = maxAmmo_;
    canShot_ = true;
    shotPerSec_ = 5u;
    shootMaxCooldown_ = 1.0f / static_cast<float> (shotPerSec_);
    shootCooldownTimer_ = 0.0f;

    actionKey_ = DIK_SPACE;
    state_ = std::bind(&Player::Descend, this);
}

void Player::Update()
{
    Imgui();

    Bulletdelete();

    UpdateCooltime_();

    state_();

    UpdatePosition();

    AddAmmo();

    // 弾の処理
    Attack();
    for (PlayerBullet* bullet : bullets_)
    {
        bullet->Update(screw_->GetCenter());
        if(!bullet->IsDead())
            CollisionManager::GetInstance()->SetCollider(bullet);
    }
}

void Player::Draw(const ViewProjection& _vp)
{
    obj_->Draw(worldTransform_, _vp);
    for (PlayerBullet* bullet : bullets_)
        bullet->Draw(_vp);
}

void Player::Rotate()
{
    Vector3 target = screw_->GetCenter();
    // 軸は{0,1,0}方向固定なのでplayerに合わせる
    target.y = position_;

    // y軸だけ向かせる
    Vector3 dir = target - worldTransform_.translation_;
    dir =  dir.Normalize();
    worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);
}


void Player::Attack()
{
    // 弾の最大数以上なら早期リターン
    if (remainingAmmo_ <= 0 || !canShot_)
        return;

    if (input_->PushKey(actionKey_)) {

        // 弾の速度
        const float kBulletSpeed = -1.0f;
        Vector3 velocity(0, 0, kBulletSpeed);

        velocity = TransformNormal(velocity, worldTransform_.matWorld_);
        velocity = GetWorldPosition() - screw_->GetCenter();
        velocity = velocity.Normalize() * kBulletSpeed;
        velocity.y = 0;

        // 弾を生成し、初期化
        PlayerBullet* newBullet = new PlayerBullet();
        newBullet->Initialize(worldTransform_.translation_, velocity);

        // 弾を登録する
        bullets_.push_back(newBullet);
        --remainingAmmo_;
        canShot_ = false;
    }
}

void Player::Bulletdelete()
{
    // デスフラグの立った弾を削除
    bullets_.remove_if([](PlayerBullet* bullet) {
        if (bullet->IsDead()) {
            delete bullet;
            return true;
        }
        return false;
        });
}

void Player::AddAmmo()
{
    if (prePositionIndex_ - positionIndex_ == -1 ||
        prePositionIndex_ - positionIndex_ == 1)
        remainingAmmo_++;
}

void Player::Imgui()
{
    if (ImGui::BeginTabBar("Player"))
    {
        if (ImGui::BeginTabItem("Player"))
        {
            ImGui::SliderFloat("Position", &position_, 0.0f, screw_->GetHeight());
            // 最大落下速度の調整ができる
            ImGui::DragFloat("MinVelocity", &minVelocity_, 0.1f);
            // 落下の加速度を調整できる
            ImGui::DragFloat("Acceleration_", &downAcceleration_, 0.01f);
            // キーを話したとｋのスピードの調整ができる
            ImGui::DragFloat("BaseAscendSpeed", &baseAscendSpeed_, 0.01f);
            ImGui::InputInt("MaxChargeTime", reinterpret_cast<int*>(&maxChargeTime_));
            ImGui::Text("chargeTime: %d", chargeTime_);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("pBullet"))
        {
            ImGui::Text("CoolTime : %f", shootMaxCooldown_);
            // 弾の発射間隔をいじれる
            if (ImGui::InputInt("shotPerSec", reinterpret_cast<int*>(&shotPerSec_)))
            {
                shootMaxCooldown_ = 1.0f / static_cast<float> (shotPerSec_);
                shootCooldownTimer_ = shootMaxCooldown_;
            }
            ImGui::InputInt("maxAmmo", reinterpret_cast<int*>(&maxAmmo_));
            if (ImGui::Button("ReloadAmmo") || Input::GetInstance()->TriggerKey(DIK_R))
                remainingAmmo_ = maxAmmo_;
            ImGui::Text("RemainingAmmo : %d", remainingAmmo_);
            ImGui::Text("bulletNum: %d", bullets_.size());
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    };
}

void Player::ShootAndCharge()
{
    if (input_->ReleaseKey(actionKey_))
    {
        velocity_ = baseAscendSpeed_ * multiplier * static_cast<float>(chargeTime_);
        chargeTime_ = 0u;
        state_ = std::bind(&Player::ReleasePowerAndAscend, this);
        return;
    };
    if (chargeTime_ < maxChargeTime_)
        chargeTime_++;
    velocity_ = 0.0f;
    Attack();
}

void Player::ReleasePowerAndAscend()
{
    // 加速度加算で速度減少
    velocity_ += -downAcceleration_ * multiplier;

    // velovityが０以下で下降フェーズ
    if (velocity_ < 0.0f )
    {
        state_ = std::bind(&Player::Descend, this);
        return;
    }
    // 上昇中にキーを押したらチャージ開始
    else if (input_->TriggerKey(actionKey_))
    {
        state_ = std::bind(&Player::ShootAndCharge, this);
        return;
    }
}

void Player::Descend()
{
    // キーを押したらチャージ開始
    if (input_->TriggerKey(actionKey_))
    {
        state_ = std::bind(&Player::ShootAndCharge, this);
        return;
    }
    // 下降
    velocity_ += -downAcceleration_ * multiplier;
    if (velocity_ < minVelocity_ * multiplier)
        velocity_ = minVelocity_ * multiplier;

}

Vector3 Player::GetWorldPosition()
{
    Vector3 worldPos;
    worldPos.x = worldTransform_.matWorld_.m[3][0];
    worldPos.y = worldTransform_.matWorld_.m[3][1];
    worldPos.z = worldTransform_.matWorld_.m[3][2];

    return worldPos;
}

void Player::UpdatePosition()
{
    float maxHeight = screw_->GetHeight();
    //  移動
    position_ += velocity_;

    //クランプ
    if (position_ > maxHeight)  position_ = 0;
    if (position_ < 0.0f)  position_ = maxHeight;

    UpdatePositionIndex();

    // スクリュー上の座標を取得
    worldTransform_.translation_ = screw_->GetPositionOnLine(position_);
    Rotate();
    worldTransform_.UpdateMatrix();

}

void Player::UpdatePositionIndex()
{
    // スクリューの一回転当たりの高さの半分を取得
    float halfHeightPerRot = screw_->GetHeightPerRot() / 2.0f;
    // 前フレームの値を取得
    prePositionIndex_ = positionIndex_;
    positionIndex_ = static_cast<uint32_t>(position_ / halfHeightPerRot);
}

void Player::UpdateCooltime_()
{

    if (canShot_)
        return;

    const float deltaTime = 1.0f / 60.0f;
    shootCooldownTimer_ -= deltaTime;
    if (shootCooldownTimer_ <= 0.0f)
    {
        shootCooldownTimer_ = shootMaxCooldown_;
        canShot_ = true;
    }

}
