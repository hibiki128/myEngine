#pragma once
#include "ParticleEmitter.h"
#include "application/Base/BaseObject.h"
class Player;
class Enemy : public BaseObject {
    enum class Behavior {
        kRoot,
        kKnockback,
    };

  public:
    /// ===================================================
    /// public method
    /// ===================================================

    void Init(const std::string className) override;

    void Update() override;

    void Draw(const ViewProjection &viewProjection) override;

    void DrawParticle(const ViewProjection &viewProjection);

    void Debug(std::string &name);

    void SetPlayer(Player *player) { player_ = player; }

    void SetPosition(const Vector3 &position) { transform_.translation_ = position; }

    bool IsDead() { return isDead_; }

    bool IsHit() { return isHit_; }

  private:
    void Move();

    void Rotation();

    void KnockBack();

    void BehaviorUpdate();

    void BehaviorRootInitialize();

    void BehaviorKnockbackInitialize();

    void BehaviorRootUpdate();

    void BehaviorKnockbackUpdate();

    Vector3 GetCenterPosition() const override;
    Vector3 GetCenterRotation() const override;

    void OnCollision([[maybe_unused]] Collider *other) override;
    void OnCollisionEnter([[maybe_unused]] Collider *other) override;

  private:
    // 次の振るまいリクエスト
    std::optional<Behavior> behaviorRequest_ = std::nullopt;

    std::unique_ptr<ParticleEmitter> hitParticle_ = nullptr;
    std::unique_ptr<ParticleEmitter> slashParticle_ = nullptr;

    std::unique_ptr<BaseObject> HPBar_;
    std::unique_ptr<BaseObject> HPObj_;

    Behavior behavior_ = Behavior::kRoot;

    Player *player_ = nullptr;

    int HP_ = 10;

    bool isHit_ = false;
    bool isEffect_ = false;
    bool isDead_ = false;
    bool isStop_ = false;

    float coolTime_ = 0.0f;
    float knockbackTimer_ = 0.0f;
    float fallSpeed = 0.0f;

    const float knockbackTMax_ = 0.2f;

    int32_t damageSE_;
    int32_t damageSE2_;
};
