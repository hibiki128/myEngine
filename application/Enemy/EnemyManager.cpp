#include "EnemyManager.h"
#include <myEngine/Frame/Frame.h>
#include <random>

void EnemyManager::Init(const Vector3& position, Player* player)
{
	position_ = position;
	player_ = player;
	
}

void EnemyManager::Update(const int maxEnemy)
{
	EnemiesUpdate(maxEnemy);
}

void EnemyManager::Draw(const ViewProjection& vp)
{
	for (auto& enemy : enemies_) {
		enemy->Draw(vp);
	}
}

void EnemyManager::DrawParticle(const ViewProjection& vp)
{
	for (auto& spawnEffect : spawnEffects_) {
		spawnEffect->UpdateOnce(vp);
		spawnEffect->Draw();
	}
}

void EnemyManager::AddEnemy(std::unique_ptr<Enemy> enemy)
{
	enemies_.push_back(std::move(enemy));
}

void EnemyManager::SpawnEnemy(const Vector3& enemyPosition)
{
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
	spawnEffect_->Initialize("spawnEffect", "debug/cube.obj");
	spawnEffect_->SetPosition(enemyPosition);
	spawnEffect_->SetActive(false);
	spawnEffects_.push_back(std::move(spawnEffect_));
}

void EnemyManager::SpawnEnemies(const Vector3& position)
{
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
	enemyPosition.y = position_.y;

	// 敵を生成
	SpawnEnemy(enemyPosition);

}

void EnemyManager::EnemiesUpdate(const int maxEnemy)
{
	const int maxEnemies = maxEnemy;

	// 敵を追加するためのタイマー更新
	spawnTimer_ += Frame::DeltaTime();

	// タイマーが spawnInterval_ を超えた場合に敵を追加
	if (spawnTimer_ >= spawnInterval_)
	{
		spawnTimer_ = 0.0f;  // タイマーリセット

		// 敵の数が5以下の場合に追加
		if (enemies_.size() < maxEnemies)
		{
			Vector3 playerPosition = position_;
			SpawnEnemies({ playerPosition.x,1.0f,playerPosition.z }); // プレイヤー周囲に敵を生成
		}
	}

	// 死亡した敵をリストから削除し、カウントを更新
	auto it = enemies_.begin();
	while (it != enemies_.end())
	{
		if ((*it)->IsDead())  // 死亡している場合
		{
			it = enemies_.erase(it);  // リストから削除
			++deadEnemiesCount_;  // 死亡した敵をカウント
		}
		else
		{
			++it;
		}
	}
	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}
}
