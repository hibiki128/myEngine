#include "Enemy.h"
#include "application/Player/Player.h"
#include "application/Player/Weapon/Weapon.h"
#include <myEngine/Frame/Frame.h>

void Enemy::Init(const std::string className) {
    BaseObject::Init(className);
    BaseObject::CreateModel("debug/Cube.obj");
    BaseObject::CreateCollider();
    Collider::SetCollisionType(CollisionType::OBB);
    Collider::SetVisible(false);
    objColor_.SetColor({1.0f, 1.0f, 0.0f, 1.0f});
    transform_.translation_ += transform_.scale_.y;
    HP_ = 10;

    hitParticle_ = std::make_unique<ParticleEmitter>();
    hitParticle_->Initialize("hit", "debug/cube.obj");
    hitParticle_->SetTexture("game/red1x1.png");

    slashParticle_ = std::make_unique<ParticleEmitter>();
    slashParticle_->Initialize("slash", "debug/plane.obj");
    slashParticle_->SetTexture("game/slash.png");

    isEffect_ = false;
}

void Enemy::Update() {
    BehaviorUpdate();

    if (HP_ <= 0) {
        isDead_ = true;
    }

    if (isStop_) {
        coolTime_ -= Frame::DeltaTime();
        if (coolTime_ < 0) {
            isStop_ = false;
        }
    }
}

void Enemy::Draw(const ViewProjection &viewProjection) {
    if (!isDead_) {

        BaseObject::Draw(viewProjection);
    }
}

void Enemy::DrawParticle(const ViewProjection &viewProjection) {
    if (!isDead_) {
    }
    ParticleCommon::GetInstance()->SetBlendMode(BlendMode::kAdd);
    hitParticle_->Draw(viewProjection);
    //ParticleCommon::GetInstance()->SetBlendMode(BlendMode::kNormal);
    slashParticle_->Draw(viewProjection);
   // ParticleCommon::GetInstance()->SetBlendMode(BlendMode::kAdd);
}

void Enemy::Debug(std::string &name) {
    if (ImGui::BeginTabItem(name.c_str())) {
        ImGui::Text("HP : %d", HP_);
        ImGui::EndTabItem();
    }
}

void Enemy::Move() {
    // プレイヤーとの位置ベクトルを計算
    Vector3 playerPosition = player_->GetPosition();    // プレイヤーの位置
    Vector3 enemyPosition = transform_.translation_;    // 敵の位置
    Vector3 direction = playerPosition - enemyPosition; // プレイヤーと敵の位置差
    Vector3 NormDirection = {0.0f, 0.0f, 0.0f};

    // ベクトルの正規化（単位ベクトルに変換）
    NormDirection = direction.Normalize();

    // 距離の計算
    float distance = direction.Length(); // ベクトルの長さを計算

    // 移動制限距離
    float stopDistance = transform_.scale_.x * 2.5f;

    // 距離が制限値より大きい場合のみ移動
    if (distance > stopDistance && !isStop_) {
        // 移動スピードを掛け算
        float speed = 0.15f;
        Vector3 movement = NormDirection * speed;

        // 敵の位置を移動させる
        transform_.translation_.x += movement.x;
        transform_.translation_.z += movement.z;
        coolTime_ = 1.0f;
    } else {
        isStop_ = true;
    }
}

void Enemy::Rotation() {
    // プレイヤーとの位置ベクトルを計算
    Vector3 playerPosition = player_->GetPosition();    // プレイヤーの位置
    Vector3 enemyPosition = transform_.translation_;    // 敵の位置
    Vector3 direction = playerPosition - enemyPosition; // プレイヤーと敵の位置差

    // 方向ベクトルから目標の回転角度を計算
    float targetRotation = std::atan2(direction.x, direction.z); // プレイヤーの方向の角度計算
    const float rotationSpeed = 0.1f;                            // 回転補間速度

    // 現在の回転角度を -π から π の範囲に正規化
    while (transform_.rotation_.y - targetRotation > std::numbers::pi_v<float>) {
        transform_.rotation_.y -= 2.0f * std::numbers::pi_v<float>;
    }
    while (transform_.rotation_.y - targetRotation < -std::numbers::pi_v<float>) {
        transform_.rotation_.y += 2.0f * std::numbers::pi_v<float>;
    }

    // スムーズな回転のための線形補間
    transform_.rotation_.y = transform_.rotation_.y + (targetRotation - transform_.rotation_.y) * rotationSpeed;
}

void Enemy::KnockBack() {
    // ノックバック処理
    if (isHit_) {
        // プレイヤーとの位置ベクトルを計算
        Vector3 playerPosition = player_->GetPosition();    // プレイヤーの位置
        Vector3 enemyPosition = transform_.translation_;    // 敵の位置
        Vector3 direction = enemyPosition - playerPosition; // プレイヤーと敵の位置差の逆方向

        // y成分を無視してxとz軸の方向ベクトルを計算
        direction.y = 0.0f;
        direction = direction.Normalize(); // ベクトルの正規化

        // ノックバックの目標位置と開始位置を設定
        float knockbackDistance = 1.0f;
        Vector3 knockbackTarget = enemyPosition + (direction * knockbackDistance);
        knockbackTarget.y = enemyPosition.y; // y軸の位置は変更しない
        Vector3 knockbackStart = enemyPosition;

        // 補間のタイマーを進める
        if (knockbackTimer_ < knockbackTMax_) {
            knockbackTimer_ += Frame::DeltaTime(); // 時間を進める
        }

        // EaseInSineで位置を補完
        Vector3 knockbackMovement = EaseInSine(knockbackStart, knockbackTarget, knockbackTimer_, knockbackTMax_);

        // 敵の位置を更新（xとzのみ変化）
        transform_.translation_.x = knockbackMovement.x;
        transform_.translation_.z = knockbackMovement.z;

        if (!isStop_) {
            coolTime_ = 1.0f;
        }

        // ノックバックが完了したらフラグをリセットする
        if (knockbackTimer_ >= knockbackTMax_) {
            isHit_ = false;
            knockbackTimer_ = 0.0f; // タイマーをリセット
            isStop_ = true;
        }
    }
}

void Enemy::BehaviorUpdate() {
    if (!isDead_) {
        if (behaviorRequest_) {
            // 振るまいを変更する
            behavior_ = behaviorRequest_.value();
            // 各振るまいごとの初期化を実行
            switch (behavior_) {
            case Behavior::kRoot:
            default:
                BehaviorRootInitialize();
                break;
            case Behavior::kKnockback:
                BehaviorKnockbackInitialize();
                break;
            }
            // 振るまいリクエストをリセット
            behaviorRequest_ = std::nullopt;
        }
        switch (behavior_) {
        case Behavior::kRoot:
        default:
            BehaviorRootUpdate();
            break;
        case Behavior::kKnockback:
            BehaviorKnockbackUpdate();
            break;
        }

        BaseObject::Update();
        hitParticle_->SetPosition(transform_.translation_);
        slashParticle_->SetPosition(transform_.translation_);
        if (isEffect_) {
            hitParticle_->UpdateOnce();
            slashParticle_->UpdateOnce();
            isEffect_ = false;
        }
    } else {
        Collider::SetCollisionEnabled(false);
    }
}

void Enemy::BehaviorRootInitialize() {
}

void Enemy::BehaviorKnockbackInitialize() {
}

void Enemy::BehaviorRootUpdate() {
    Move();
    Rotation();
    if (isHit_) {
        behaviorRequest_ = Behavior::kKnockback;
    }
}

void Enemy::BehaviorKnockbackUpdate() {
    KnockBack();
    if (!isHit_) {
        behaviorRequest_ = Behavior::kRoot;
    }
}

Vector3 Enemy::GetCenterPosition() const {
    return BaseObject::GetCenterPosition();
}

Vector3 Enemy::GetCenterRotation() const {
    return BaseObject::GetCenterRotation();
}

void Enemy::OnCollision(Collider *other) {
    // 他のEnemyとの衝突時に押し戻し処理を実行
    if (dynamic_cast<Enemy *>(other)) {
        Enemy *otherEnemy = dynamic_cast<Enemy *>(other);

        // 衝突した2つの敵の位置ベクトルを計算
        Vector3 enemyPosition = transform_.translation_;
        Vector3 otherEnemyPosition = otherEnemy->transform_.translation_;

        // 敵同士の位置差を計算（衝突方向）
        Vector3 direction = enemyPosition - otherEnemyPosition;

        // 衝突方向のベクトルを正規化
        direction.y = 0.0f; // y軸方向を無視（水平面上での押し戻し）
        direction.Normalize();

        // 押し戻し距離の設定
        float pushBackDistance = 0.025f;

        // 他の敵と自分を押し戻す処理
        Vector3 pushBackMovement = direction * pushBackDistance;

        // 自分自身と相手をそれぞれ押し戻す
        transform_.translation_ += pushBackMovement;
        otherEnemy->transform_.translation_ -= pushBackMovement; // 反対方向に押し戻す
    }
    if (dynamic_cast<Player *>(other)) {
        Player *player = dynamic_cast<Player *>(other);

        // 衝突した2つの敵の位置ベクトルを計算
        Vector3 enemyPosition = transform_.translation_;
        Vector3 playerPosition = player->GetWorldPosition();

        // 敵同士の位置差を計算（衝突方向）
        Vector3 direction = enemyPosition - playerPosition;

        // 衝突方向のベクトルを正規化
        direction.y = 0.0f; // y軸方向を無視（水平面上での押し戻し）
        direction.Normalize();

        // 押し戻し距離の設定
        float pushBackDistance = 0.09f;

        // 他の敵と自分を押し戻す処理
        Vector3 pushBackMovement = direction * pushBackDistance;

        // 自分自身と相手をそれぞれ押し戻す
        transform_.translation_ += pushBackMovement;
    }
}

void Enemy::OnCollisionEnter(Collider *other) {
    if (HP_ > 0) {
        if (dynamic_cast<Weapon *>(other)) {
            --HP_;
            isHit_ = true;
            isEffect_ = true;
        }
    }
}
