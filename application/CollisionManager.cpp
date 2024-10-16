#include "CollisionManager.h"
#include "Collider.h"
#include "myMath.h"

CollisionManager* CollisionManager::GetInstance()
{
    static CollisionManager instance;
    return &instance;
}

void CollisionManager::SetCollider(Collider* _collider)
{
    colliders_.push_back(_collider);
}

void CollisionManager::CheckAllCollisions()
{
    for (auto itA = colliders_.begin(); itA != colliders_.end(); itA++)
    {
        for (auto itB = std::next(itA); itB != colliders_.end(); itB++)
        {
            if ((*itA)->GetMask() & (*itB)->GetAtrribute_() ||
                (*itB)->GetMask() & (*itA)->GetAtrribute_())
                    CheckCollisionPair(*itA, *itB);
        }
    }
}

uint32_t CollisionManager::GetAtttibute(const std::string& _atrribute)
{
    auto it = map_.find(_atrribute);
    if (it != map_.end())
        return it->second;

    map_[_atrribute] = static_cast <uint32_t>(map_.size() + 1);
    uint32_t result = map_[_atrribute];
    return result;
}

uint32_t CollisionManager::GetMask(const std::string& _atrribute)
{   
    return ~GetAtttibute(_atrribute);
}

void CollisionManager::CheckCollisionPair(Collider* _colliderA, Collider* _colliderB)
{
    uint32_t collision = static_cast<uint32_t>(_colliderA->GetBoundingBox()) | static_cast<uint32_t>(_colliderB->GetBoundingBox());
    Vector3 wPosA = _colliderA->GetWorldPosition();
    Vector3 wPosB = _colliderB->GetWorldPosition();

    switch (collision)
    {
    // 球と球
    case 0: 
    {
        float radiusA = _colliderA->GetRadius();
        float radiusB = _colliderB->GetRadius();

        float distance = (wPosA - wPosB).Length();
        if (distance < radiusA + radiusB)
        {
            _colliderA->OnCollision();
            _colliderB->OnCollision();
        }
        break;
    }
    // 球と箱
    case 1:
        /// 一旦保留
    break;
    // 箱と箱
    case 2:
    {
        /// 一旦保留
        break;
    }
    default:
        break;
    }
}
