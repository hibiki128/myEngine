#include "EnemyManager.h"
#include <myEngine/Frame/Frame.h>
#include <random>

void EnemyManager::Init(const Vector3 &position, Player *player) {
    position_ = position;
    player_ = player;
}

void EnemyManager::Update() {
    EnemiesUpdate();
}

void EnemyManager::Draw(const ViewProjection &vp) {
    for (auto &enemy : enemies_) {
        enemy->Draw(vp);
    }
}

void EnemyManager::DrawParticle(const ViewProjection &vp) {
    for (auto &enemy : enemies_) {
        enemy->DrawParticle(vp);
    }
    ParticleCommon::GetInstance()->SetBlendMode(BlendMode::kAdd);
    for (auto &spawnEffect : spawnEffects_) {

        spawnEffect->Draw(vp);
    }
    for (auto &deathParticle : deathParticles_) {
        // ParticleCommon::GetInstance()->SetBlendMode(BlendMode::kNormal);
        deathParticle->Draw(vp);
        // ParticleCommon::GetInstance()->SetBlendMode(BlendMode::kAdd);
    }
}

void EnemyManager::Debug() {
    ImGui::Begin("敵達");
    if (ImGui::BeginTabBar("敵")) {

        int index = 0;
        for (auto &enemy : enemies_) {
            std::string name = "敵" + std::to_string(index); // 敵0, 敵1, 敵2, ...
            enemy->Debug(name);                              // EnemyクラスのDebug関数を呼び出して名前を設定する

            index++;
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
}

void EnemyManager::AddEnemy(std::unique_ptr<Enemy> enemy) {
    enemies_.push_back(std::move(enemy));
}

void EnemyManager::SpawnEnemy(const Vector3 &enemyPosition) {
    // 敵の生成
    std::unique_ptr<Enemy> enemy;
    enemy = std::make_unique<Enemy>();
    // 敵キャラモデル
    enemy->Init("enemy");
    enemy->SetPosition(enemyPosition);
    enemy->SetPlayer(player_);
    // 敵を追加
    AddEnemy(std::move(enemy));

    // パーティクル
    std::unique_ptr<ParticleEmitter> spawnEffect_;
    spawnEffect_ = std::make_unique<ParticleEmitter>();
    spawnEffect_->Initialize("spawnEffect", "debug/ICO.obj");
    spawnEffect_->SetPosition(enemyPosition);
    spawnEffect_->UpdateOnce();
    spawnEffects_.push_back(std::move(spawnEffect_));
}

void EnemyManager::SpawnEnemies(const Vector3 &position) {
    const float radius = 30.0f;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> angleDist(0.0, 360.0);

    std::uniform_real_distribution<> distDist(0.0, radius);

    float angle = float(angleDist(gen));

    float dist = float(distDist(gen));

    // ランダムな位置を計算
    Vector3 enemyPosition;
    enemyPosition.x = position_.x + dist * cosf(degreesToRadians(angle));
    enemyPosition.z = position_.z + dist * sinf(degreesToRadians(angle));
    enemyPosition.y = 1.0f;

    // 敵を生成
    SpawnEnemy(enemyPosition);
}

void EnemyManager::EnemiesUpdate() {
    // 敵を追加するためのタイマー更新
    spawnTimer_ += Frame::DeltaTime();

    // ウェーブの進行管理
    if (enemies_.empty() && currentWave_ < maxWaves_ && !isSpawning_) {
        currentWave_++; // 次のウェーブへ
        spawnCount_ = 0;
        isSpawning_ = true;
        nextSpawnTime_ = spawnTimer_ + spawnDelay_; // 最初のスポーン時間設定
    }

    // ウェーブ中の敵スポーン処理
    if (isSpawning_ && spawnTimer_ >= nextSpawnTime_) {
        Vector3 playerPosition = position_;
        SpawnEnemies({playerPosition.x, 1.0f, playerPosition.z}); // 1体ずつスポーン
        spawnCount_++;

        if (spawnCount_ >= enemiesPerWave_) {
            isSpawning_ = false; // 指定数スポーンしたら完了
        } else {
            nextSpawnTime_ = spawnTimer_ + spawnDelay_; // 次のスポーン時間設定
        }
    }

    // 死亡した敵をリストから削除し、カウントを更新
    auto it = enemies_.begin();
    while (it != enemies_.end()) {
        if ((*it)->IsDead()) {
            std::unique_ptr<ParticleEmitter> deathParticle;
            deathParticle = std::make_unique<ParticleEmitter>();
            deathParticle->Initialize("death", "Enemy/deathParticle.obj");
            deathParticle->SetPosition((*it)->GetWorldPosition());
            deathParticle->UpdateOnce();
            deathParticles_.push_back(std::move(deathParticle));

            it = enemies_.erase(it); // リストから削除
            ++deadEnemiesCount_;     // 死亡した敵をカウント
        } else {
            ++it;
        }
    }

    // 残りの敵を更新
    for (auto &enemy : enemies_) {
        enemy->Update();
    }
}
