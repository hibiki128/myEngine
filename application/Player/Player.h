#pragma once
#include "Easing.h"
#include "GlobalVariables.h"
#include "Input.h"
#include "ParticleEmitter.h"
#include "Vector3.h"
#include "Weapon/Weapon.h"
#include "application/Base/BaseObject.h"
#include "application/Shake/Shake.h"
#include"Audio.h"

class FollowCamera;
class Player : public BaseObject {
  private:
    enum class Behavior {
        kRoot,
        kAttack,
    };

  public:
    /// ===================================================
    /// public method
    /// ===================================================

    void Init(const std::string className) override;

    void Update() override;

    void Draw(const ViewProjection &viewProjection) override;

    void DrawParticle(const ViewProjection &viewProjection);

    void DrawCrack(const ViewProjection &viewProjection);

    void imgui();

    void Debug();

    void SetCamera(FollowCamera *camera) { camera_ = camera; }

    Vector3 GetPosition() { return transform_.translation_; }
    Vector3 GetRotation() { return transform_.rotation_; }
    Matrix4x4 GetWorldMatrix() { return transform_.matWorld_; }

    Vector3 GetCenterPosition() const override;
    Vector3 GetCenterRotation() const override;

    void OnCollision([[maybe_unused]] Collider *other) override;
    void OnCollisionEnter([[maybe_unused]] Collider *other) override;
    int GetComboStage() { return comboStage_; }
    int GetDamage() { return damage; }
    float GetknockValue() { return knockBackValue; }
    bool IsHit() { return weapon_->IsHit(); }
    bool IsAlive() { return isAlive_; }

    void SetIsHit(bool flag) { weapon_->SetIsHit(flag); }
    void SetVp(ViewProjection *vp) { shake_->Initialize(vp); }
    void SetVpp(ViewProjection *vp) { vp_ = vp; }

  private:
    /// ===================================================
    /// private method
    /// ===================================================

    void Move();

    void Rotation();

    void AffterEffect();

    void ArmFollow();

    void Attack();

    void NormalAttack();

    void UpAttack();

    void DownAttack();

    void RowlingAttack();

    void BehaviorRootInitialize();

    void BehaviorAttackInitialize();

    void BehaviorRootUpdate();

    void BehaviorAttackUpdate();

    void HandleCombo();

    void ProcessComboInput();

    void Frash();

    void PlaySE();

  private:
    FollowCamera *camera_;
    std::unique_ptr<Weapon> weapon_;
    std::unique_ptr<ParticleEmitter> afterImageEmitter_;
    std::unique_ptr<ParticleEmitter> quake_;
    std::unique_ptr<ParticleEmitter> deathParticle_;
    std::unique_ptr<Object3d> shadow_;
    std::unique_ptr<BaseObject> crack_;
    std::unique_ptr<BaseObject> HPBar_;
    std::unique_ptr<BaseObject> HPObj_;

    // シェイク
    std::unique_ptr<Shake> shake_;

    ViewProjection *vp_;

    // プレイヤーの部位
    WorldTransform R_arm_wt;
    WorldTransform L_arm_wt;
    WorldTransform Shadow_;
    std::unique_ptr<Object3d> R_armModel_;
    std::unique_ptr<Object3d> L_armModel_;
    ObjColor objColor_;
    Vector4 color_;

    // 次の振るまいリクエスト
    std::optional<Behavior> behaviorRequest_ = std::nullopt;
    Behavior behavior_ = Behavior::kRoot;

    Vector3 move = {0.0f, 0.0f, 0.0f};
    Vector3 acce = {0.1f, 0.0f, 0.1f};
    Vector3 speed = {0.0f, 0.0f, 0.0f};
    Vector3 R_arm_offset = {0.0f, 0.0f, 0.0f};
    Vector3 L_arm_offset = {0.0f, 0.0f, 0.0f};

    float dashCoolTime_;
    float dashSpeed_;
    float kMoveSpeed = 0.3f;
    float kDashSpeed = 1.75f;
    float kDashDecay = 0.97f;

    int damage = 1;
    float knockBackValue = 1.0f;
    int comboStage_ = 0; // 現在のコンボステージ
    int HP_ = 20;
    float invincibleTime = 0.0f;
    float startCoolTime_ = 1.0f;
    float comboTimer_ = 0.0f; // コンボ入力待機タイマー
    float attackTimer_ = 0.0f;
    float alpha_ = 0.0f;
    float red_ = 1.0f;
    bool isNextAttack_ = false;
    bool isJKeyPressed_ = false;
    bool isCrack_ = false;
    bool isAlive_ = true;
    bool isAttack_ = false;
    Vector3 startAngle;
    Vector3 endAngle;
    Vector3 startPosition;
    Vector3 endPosition;
    float easeTMax;
    float fallSpeed = 0.0f;

    GlobalVariables *globalVariables = nullptr;
    const char *groupName = nullptr;

    int32_t crackSE_;
    int32_t preAttackSE_;
    int32_t preAttackSE2_;
    int32_t damageSE_;
};
