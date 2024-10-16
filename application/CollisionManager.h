#pragma once
#include <list>
#include <unordered_map>
#include <string>

class Collider;
class CollisionManager {
public:
	static CollisionManager* GetInstance();
	void SetCollider(Collider* _collider);
	void ListReset() { colliders_.clear(); };

	void CheckAllCollisions();

	uint32_t GetAtttibute(const std::string& _atrribute);
	uint32_t GetMask(const std::string& _atrribute);

private:
	void CheckCollisionPair(Collider* _colliderA, Collider* _colliderB);

	std::list<Collider*> colliders_;
	std::unordered_map<std::string, uint32_t> map_;
};