#pragma once

#include "Object3d.h"
#include "WorldTransform.h"
#include "application/player/Player.h"
#include "application/player/PlayerBullet.h"
#include <memory>
#include <list>
#include <functional>
class Input;
class Screw;
class Player
{
public:
    ~Player();

    void Initialize();
    void Update();
    void Draw(const ViewProjection& _vp );

    void SetScrewPtr(const Screw* _screw) { screw_ = _screw; }
    Vector3 GetWorldPosition();
private:

    void UpdatePosition();
    void UpdatePositionIndex();
    void UpdateCooltime_();


    void Rotate();

    void Attack();
    void Bulletdelete();
    void AddAmmo();

    void Imgui();


    /// キーを押してるときの処理
    /// 射撃とチャージ
    void ShootAndCharge();

    /// キーを離したときの処理
    /// チャージの開放と上昇
    void ReleasePowerAndAscend();

    /// 何もしてないときの処理
    /// 下降
    void Descend();


    Input*                      input_              = nullptr;
    const Screw*                screw_              = nullptr;
    std::unique_ptr<Object3d>   obj_                = nullptr;
    WorldTransform              worldTransform_     = {};


    uint32_t                    positionIndex_      = 0u;   //
    uint32_t                    prePositionIndex_   = 0u;   //
    uint32_t                    chargeTime_         = 0u;   // チャージしたフレーム数
    uint32_t                    maxChargeTime_      = 0u;   // 最大チャージ

    float                       position_           = 0.0f; // スクリュー上のｙ座標
    float                       velocity_           = 0.0f; // 毎フレーム加算する
    float                       baseAscendSpeed_    = 0.0f;// 上昇時の基準の速度
    float                       downAcceleration_   = 0.0f; // 下降時の加速度
    float                       minVelocity_        = 0.0f; // 下降時の最小速度
    float                       multiplier          = 1.0f / 10000.0f;

    // bullet関係
    float                       shootMaxCooldown_   = 0.0f; // 射撃クールタイム
    float                       shootCooldownTimer_ = 0.0f; // クールタイム用タイマー
    uint32_t                    shotPerSec_         = 10;   // 秒あたりに撃てる弾の数
    uint32_t                    maxAmmo_            = 20u;  // 弾の最大数
    uint32_t                    remainingAmmo_      = 0u;   // 弾の残弾数
    std::list<PlayerBullet*>    bullets_            = {};
    bool                        canShot_           = false;// 射撃用フラグ

    BYTE                        actionKey_          = 0u;

    std::function<void()> state_;
};