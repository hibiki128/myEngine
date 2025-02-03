#pragma once
#include "ParticleEmitter.h"
#include "application/Enemy/Enemy.h"
class Player;
class EnemyManager {
  public:
    void Init(const Vector3 &position, Player *player);
    void Update();
    void Draw(const ViewProjection &vp);
    void DrawParticle(const ViewProjection &vp);
    void Debug();
    int GetDeadCount() { return deadEnemiesCount_; }
    void SetPosition(Vector3 position) { position_ = position; }

  private:
    void AddEnemy(std::unique_ptr<Enemy> enemy);

    void SpawnEnemy(const Vector3 &enemyPosition);

    void SpawnEnemies(const Vector3 &position);

    void EnemiesUpdate();

  private:
    // 敵
    std::list<std::unique_ptr<Enemy>> enemies_;
    std::vector<std::unique_ptr<ParticleEmitter>> spawnEffects_;
    std::vector<std::unique_ptr<ParticleEmitter>> deathParticles_;

    float spawnTimer_ = 0.0f;          // 敵を追加するためのタイマー
    const float spawnInterval_ = 1.0f; // 敵を追加する間隔（1秒）

    int deadEnemiesCount_ = 0; // 死亡した敵のカウント
    Vector3 position_;

    Player *player_;
    int currentWave_ = 0;          // 現在のウェーブ
    const int maxWaves_ = 2;       // 最大ウェーブ数
    const int enemiesPerWave_ = 5; // 各ウェーブでの敵の数
    float spawnDelay_ = 1.0f;    // 敵スポーンの間隔（秒）
    float nextSpawnTime_ = 0.0f; // 次にスポーンする時間
    int spawnCount_ = 0;         // 何体スポーンしたか
    bool isSpawning_ = false;    // 現在ウェーブ中にスポーン中か
};
