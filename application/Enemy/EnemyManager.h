#pragma once
#include"application/Enemy/Enemy.h"
#include"ParticleEmitter.h"
class Player;
class EnemyManager
{
public:
	void Init(const Vector3& position, Player* player);
	void Update(const int maxEnemy);
	void Draw(const ViewProjection& vp);
	void DrawParticle(const ViewProjection& vp);
	int GetDeadCount() { return deadEnemiesCount_; }

private:
	void AddEnemy(std::unique_ptr<Enemy> enemy);

	void SpawnEnemy(const Vector3& enemyPosition);

	void SpawnEnemies(const Vector3& position);

	void EnemiesUpdate(const int maxEnemy);
private:
	// 敵
	std::list<std::unique_ptr<Enemy>> enemies_;
	std::vector<std::unique_ptr<ParticleEmitter>> spawnEffects_;

	float spawnTimer_ = 0.0f;  // 敵を追加するためのタイマー
	const float spawnInterval_ = 1.0f; // 敵を追加する間隔（1秒）

	int deadEnemiesCount_ = 0;  // 死亡した敵のカウント
	Vector3 position_;

	Player* player_;
};

