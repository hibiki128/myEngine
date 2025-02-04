#define NOMINMAX
#include "Player.h"
#include "application/Camera/FollowCamera.h"
#include "application/Enemy/Enemy.h"
#include "myEngine/Frame/Frame.h"
#include <cstdlib>
#include <ctime>

void Player::Init(const std::string className) {
    BaseObject::Init(className);
    BaseObject::CreateModel("debug/Cube.obj");
    BaseObject::CreateCollider();
    transform_.translation_ += transform_.scale_.y;

    R_arm_wt.Initialize();
    L_arm_wt.Initialize();

    R_arm_wt.scale_ = {0.5f, 0.5f, 0.5f};
    L_arm_wt.scale_ = {0.5f, 0.5f, 0.5f};

    R_armModel_ = std::make_unique<Object3d>();
    R_armModel_->Initialize("debug/Cube.obj");
    L_armModel_ = std::make_unique<Object3d>();
    L_armModel_->Initialize("debug/Cube.obj");

    shadow_ = std::make_unique<Object3d>();
    shadow_->Initialize("shadow/shadow.obj");
    Shadow_.scale_ = {1.5f, 1.5f, 1.5f};

    weapon_ = std::make_unique<Weapon>();
    weapon_->Init("Weapon");
    weapon_->SetParent(R_arm_wt);
    weapon_->SetScale({2.0f, 2.0f, 2.0f});

    afterImageEmitter_ = std::make_unique<ParticleEmitter>();
    afterImageEmitter_->Initialize("afterImage", "debug/Cube.obj");

    quake_ = std::make_unique<ParticleEmitter>();
    quake_->Initialize("quake", "debug/Cube.obj");
    quake_->SetTexture("debug/ground.png");

    crack_ = std::make_unique<BaseObject>();
    crack_->Init("crack");
    crack_->CreateModel("debug/plane.obj");
    crack_->SetTexture("game/groundCrack.png");
    crack_->SetRotationX(degreesToRadians(-90.0f));
    crack_->SetScale({10.0f, 10.0f, 10.0f});
    crack_->SetLighting(false);
    crack_->SetObjColor({red_, 0.0f, 0.0f, alpha_});

    shake_ = std::make_unique<Shake>();

    behaviorRequest_ = Behavior::kRoot;
    behavior_ = Behavior::kRoot;

    color_ = {1.0f, 1.0f, 1.0f, 1.0f};

    objColor_.Initialize();
    objColor_.SetColor(color_);

    startAngle = {0.0f, 0.0f, 0.0f};
    endAngle = {0.0f, 0.0f, 0.0f};

    startCoolTime_ = 1.0f;

    HPBar_ = std::make_unique<BaseObject>();
    HPObj_ = std::make_unique<BaseObject>();
    HPBar_->Init("HPBar");
    HPObj_->Init("HPObj");
    HPBar_->CreateModel("HP/HPBar.obj");
    HPObj_->CreateModel("HP/HP.obj");
    HPObj_->SetTexture("debug/white1x1.png");
    HPObj_->SetObjColor({0.0f, 0.0f, 1.0f, 1.0f});

    // HPObj_->SetParent(HPBar_->GetWorldTransform());

    groupName = "NormalAttack";
    globalVariables = GlobalVariables::GetInstance();
    globalVariables->CreateGroup(groupName);
    globalVariables->AddItem(groupName, "startAngle", startAngle);
    globalVariables->AddItem(groupName, "endAngle", endAngle);
    globalVariables->AddItem(groupName, "easeTMax", easeTMax);
    groupName = "UpAttack";
    globalVariables = GlobalVariables::GetInstance();
    globalVariables->CreateGroup(groupName);
    globalVariables->AddItem(groupName, "startAngle", startAngle);
    globalVariables->AddItem(groupName, "endAngle", endAngle);
    globalVariables->AddItem(groupName, "easeTMax", easeTMax);
    globalVariables->AddItem(groupName, "startPosition", startPosition);
    globalVariables->AddItem(groupName, "endPosition", endPosition);
    groupName = "DownAttack";
    globalVariables = GlobalVariables::GetInstance();
    globalVariables->CreateGroup(groupName);
    globalVariables->AddItem(groupName, "startAngle", startAngle);
    globalVariables->AddItem(groupName, "endAngle", endAngle);
    globalVariables->AddItem(groupName, "easeTMax", easeTMax);
    globalVariables->AddItem(groupName, "startPosition", startPosition);
    globalVariables->AddItem(groupName, "endPosition", endPosition);
    groupName = "RowlingAttack";
    globalVariables = GlobalVariables::GetInstance();
    globalVariables->CreateGroup(groupName);
    globalVariables->AddItem(groupName, "startAngle", startAngle);
    globalVariables->AddItem(groupName, "endAngle", endAngle);
    globalVariables->AddItem(groupName, "easeTMax", easeTMax);

    deathParticle_ = std::make_unique<ParticleEmitter>();
    deathParticle_->Initialize("death", "Enemy/deathParticle.obj");

    crackSE_ = Audio::GetInstance()->LoadWave("crack.wav");
    damageSE_ = Audio::GetInstance()->LoadWave("damage.wav");
    preAttackSE_ = Audio::GetInstance()->LoadWave("preAttack1.wav");
    preAttackSE2_ = Audio::GetInstance()->LoadWave("preAttack2.wav");
    dashSE_ = Audio::GetInstance()->LoadWave("dash.wav");

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void Player::Update() {
    PlaySE();
    HPObj_->SetScale({2.0f, 1.0f, HP_ / 10.0f});
    if (HP_ <= 0 && isAlive_) {
        deathParticle_->SetPosition(GetCenterPosition());
        deathParticle_->UpdateOnce();
        isAlive_ = false;
        HPObj_->SetScale({2.0f, 1.0f, 0.0f});
    }
    if (isAlive_) {

        if (behaviorRequest_) {
            // 振るまいを変更する
            behavior_ = behaviorRequest_.value();
            // 各振るまいごとの初期化を実行
            switch (behavior_) {
            case Behavior::kRoot:
            default:
                BehaviorRootInitialize();
                break;
            case Behavior::kAttack:
                BehaviorAttackInitialize();
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
        case Behavior::kAttack:
            BehaviorAttackUpdate();
            break;
        }

        if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
            comboTimer_ = 1.0f;
            behaviorRequest_ = Behavior::kAttack;
        }

        if (isCrack_) {
            crack_->SetWorldPosition({GetCenterPosition().x, 0.1f, GetCenterPosition().z});
            alpha_ = 1.0f;
            red_ = 0.4f;
            quake_->SetPosition({GetCenterPosition().x, 0.0f, GetCenterPosition().z});
            quake_->UpdateOnce();
            shake_->StartShake();
            Audio::GetInstance()->PlayWave(crackSE_, 0.2f);
            isCrack_ = false;
        }

        if (alpha_ > 0.0f) {
            alpha_ -= 1.0f / 60.0f;
            red_ -= 5.0f / 60.0f;
        } else {
            alpha_ = 0.0f;
            red_ = 0.0f;
        }

        crack_->SetObjColor({red_, 0.0f, 0.0f, alpha_});

        if (invincibleTime > 0) {
            invincibleTime -= 1.0f / 60.0f;
        } else {
            invincibleTime = 0;
        }
        if (startCoolTime_ > 0) {
            startCoolTime_ -= 1.0f / 60.0f;
        }

        // 基礎更新
        BaseObject::Update();
        // 腕追従
        ArmFollow();
        afterImageEmitter_->Update();
        weapon_->ParticleUpdate();

        Shadow_.translation_ = Vector3(transform_.translation_.x, 0.1f, transform_.translation_.z);
        // 武器更新
        weapon_->Update();
        crack_->Update();
        shake_->Update();
        R_arm_wt.UpdateMatrix();
        L_arm_wt.UpdateMatrix();

        HPBar_->Update();
        HPObj_->Update();
        HPBar_->SetWorldPosition({transform_.translation_.x, transform_.translation_.y + 2.0f, transform_.translation_.z});
        HPObj_->SetWorldPosition({transform_.translation_.x - 2.0f, transform_.translation_.y + 2.0f, transform_.translation_.z});
        Frash();
    } else {
        BaseObject::SetCollisionEnabled(false);
        weapon_->SetCollisionEnabled(false);
    }
}

void Player::Draw(const ViewProjection &viewProjection) {
    if (isAlive_) {
        BaseObject::Draw(viewProjection);
        R_armModel_->Draw(R_arm_wt, viewProjection, &objColor_);
        L_armModel_->Draw(L_arm_wt, viewProjection, &objColor_);
        shadow_->Draw(Shadow_, viewProjection);
        weapon_->Draw(viewProjection);
    }

    HPBar_->Draw(viewProjection);
    HPObj_->Draw(viewProjection);
}

void Player::DrawParticle(const ViewProjection &viewProjection) {
    if (isAlive_) {
        if (behavior_ == Behavior::kRoot) {
            weapon_->SetCount(0);
        }
        if (behavior_ == Behavior::kAttack) {
            weapon_->SetCount(1);
        }

        afterImageEmitter_->Draw(viewProjection);
        // ParticleCommon::GetInstance()->SetBlendMode(BlendMode::kNormal);
        quake_->Draw(viewProjection);
        // ParticleCommon::GetInstance()->SetBlendMode(BlendMode::kAdd);
        weapon_->DrawParticle(viewProjection);
    }
    deathParticle_->Draw(viewProjection);
}

void Player::DrawCrack(const ViewProjection &viewProjection) {
    if (isAlive_) {
        crack_->Draw(viewProjection);
    }
}

void Player::imgui() {
    if (ImGui::BeginTabBar("player")) {
        if (ImGui::BeginTabItem("プレイヤー")) {
            ImGui::DragFloat3("速度", &speed.x, 0.01f);
            ImGui::DragFloat("動く速度", &kMoveSpeed, 0.01f);
            ImGui::DragFloat("ダッシュ速度", &kDashSpeed, 0.01f);
            ImGui::DragFloat("ダッシュ減衰率", &kDashDecay, 0.01f);
            ImGui::Text("コンボ数 : %d", comboStage_);
            ImGui::Text("秒数 : %f", attackTimer_);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::Begin("腕");
    if (ImGui::BeginTabBar("R_arm")) {
        if (ImGui::BeginTabItem("右腕")) {
            ImGui::DragFloat3("位置", &R_arm_offset.x, 0.1f);
            ImGui::DragFloat3("回転", &R_arm_wt.rotation_.x, 0.1f);
            ImGui::DragFloat3("大きさ", &R_arm_wt.scale_.x, 0.1f);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    if (ImGui::BeginTabBar("L_arm")) {
        if (ImGui::BeginTabItem("左腕")) {
            ImGui::DragFloat3("位置", &L_arm_offset.x, 0.1f);
            ImGui::DragFloat3("回転", &L_arm_wt.rotation_.x, 0.1f);
            ImGui::DragFloat3("大きさ", &L_arm_wt.scale_.x, 0.1f);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    HPBar_->DebugImGui();
    HPObj_->DebugImGui();

    afterImageEmitter_->imgui();
}
void Player::Debug() {
    BaseObject::DebugImGui();
    weapon_->Debug();
}
#pragma region 状態

void Player::BehaviorRootInitialize() {
    R_arm_wt.rotation_.x = 0.0f;
    R_arm_wt.rotation_.y = 0.0f;
    R_arm_wt.rotation_.z = 0.0f;
    transform_.rotation_.x = 0.0f;
    transform_.rotation_.z = 0.0f;
    fallSpeed = 0.0f;
}

void Player::BehaviorAttackInitialize() {
    dashSpeed_ = 0.0f;
}

void Player::BehaviorRootUpdate() {
    // 移動処理
    Move();

    // 回転処理
    Rotation();

    // 腕の回転をプレイヤーの回転に合わせる
    R_arm_wt.rotation_.y = transform_.rotation_.y;

    // 攻撃タイマーをリセット
    attackTimer_ = 0.0f;

    // 落下処理（transform_.translation_.y が 1.0f より大きければ落下）
    if (transform_.translation_.y > 1.0f) {
        // 重力の加速度（例えば -9.8f を適用）
        const float gravity = -9.8f * 3.0f;

        // 前回のフレームの落下速度を加速
        fallSpeed += gravity * Frame::DeltaTime() * 1.5f; // 速度は加速度に応じて増加

        // 現在の速度でy座標を更新
        transform_.translation_.y += fallSpeed * Frame::DeltaTime(); // 速度 * 経過時間

        // 1.0f に達したら y を 1.0f に固定
        if (transform_.translation_.y <= 1.0f) {
            transform_.translation_.y = 1.0f;
            fallSpeed = 0.0f; // 着地時に速度をリセット
        }
    }

    // 残像処理
    AffterEffect();
}

void Player::BehaviorAttackUpdate() {

    // コンボ処理
    HandleCombo();

    // 攻撃アニメーションやエフェクトを開始
    Attack();

    // 攻撃が終了し、コンボの入力が1秒間ない場合にkRootに戻す
    if (comboTimer_ > 0.0f) {
        comboTimer_ -= Frame::DeltaTime();
    } else {
        // 1秒間入力がない場合はkRootに戻る
        comboStage_ = 0;
        behaviorRequest_ = Behavior::kRoot;
    }

    afterImageEmitter_->SetCount(0);
}

#pragma endregion

#pragma region 攻撃関数

void Player::HandleCombo() {
    if (comboTimer_ > 0.0f) {
        // コンボタイマーが残っている間はコンボ入力を受け付ける
        if (Input::GetInstance()->TriggerKey(DIK_SPACE) && isNextAttack_) {
            // 攻撃入力があったらコンボを進める
            ProcessComboInput();
            isNextAttack_ = false; // 次の攻撃が開始されたのでフラグをリセット
            // R_arm_wt.rotation_.y = transform_.rotation_.y - 0.6f;
            attackTimer_ = 0.0f;
        } else if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
            // `J` が押されていて `isNextAttack_` が `false` の場合、入力をキープする
            isJKeyPressed_ = true;
        }
    }
    // `J` キーがキープされていた場合、次の攻撃を開始
    if (isJKeyPressed_ && isNextAttack_) {
        ProcessComboInput();
        isJKeyPressed_ = false; // キープ状態をリセット
    }
    // WASD のいずれかが押された場合、キープ状態を解除
    if (Input::GetInstance()->TriggerKey(DIK_W) ||
        Input::GetInstance()->TriggerKey(DIK_A) ||
        Input::GetInstance()->TriggerKey(DIK_S) ||
        Input::GetInstance()->TriggerKey(DIK_D)) {
        isJKeyPressed_ = false;
    }
}

void Player::ProcessComboInput() {
    // 現在のコンボステージを進める
    if (comboStage_ < 3) {
        comboStage_++;
        attackTimer_ = 0.0f;
        comboTimer_ = 1.0f; // コンボが続いているのでタイマーをリセット
    }
}
void Player::Frash() {
    if (invincibleTime > 0) {
        float blinkInterval = 0.1f; // チカチカする間隔（秒）
        float alpha = (fmod(invincibleTime, blinkInterval * 2) < blinkInterval) ? 0.0f : 1.0f;

        BaseObject::SetAlpha(alpha);
        weapon_->SetAlpha(alpha);
        objColor_.SetAlpha(alpha);
    } else {
        BaseObject::SetAlpha(1.0f);
        weapon_->SetAlpha(1.0f);
        objColor_.SetAlpha(1.0f);
    }
}

void Player::PlaySE() {
    if (isAttack_) {
        // 50% の確率で preAttackSE_ または preAttackSE2_ を選択
        int randomIndex = std::rand() % 2;
        int selectedSE = (randomIndex == 0) ? preAttackSE_ : preAttackSE2_;

        // 選択されたSEを再生
        Audio::GetInstance()->PlayWave(selectedSE, 0.15f);

        isAttack_ = false;
    }
}

void Player::Attack() {
    switch (comboStage_) {
    case 0:
        NormalAttack();
        break;
    case 1:
        UpAttack();
        break;
    case 2:
        DownAttack();
        break;
    case 3:
        RowlingAttack();
        break;
    }
}

void Player::NormalAttack() {
    groupName = "NormalAttack";
    damage = 1;
    knockBackValue = 0.5f;
    // 補間するための開始角度と終了角度を設定
    startAngle = globalVariables->GetVector3Value(groupName, "startAngle");
    endAngle = globalVariables->GetVector3Value(groupName, "endAngle");
    easeTMax = globalVariables->GetFloatValue(groupName, "easeTMax");
    startAngle.y = transform_.rotation_.y - startAngle.y; // 初期角度
    endAngle.y = transform_.rotation_.y - endAngle.y;     // 最終角度a
    // easeTMax = 1.0f;

    if (attackTimer_ == 0) {
        isAttack_ = true;
    }
    if (attackTimer_ < easeTMax) {
        attackTimer_ += Frame::DeltaTime(); // 時間を進める
    }

    // 0.5秒間で補間を適用
    R_arm_wt.rotation_ = EaseInOutQuad<Vector3>(startAngle, endAngle, attackTimer_, easeTMax); // 0.5秒で補間

    // タイマーが0.5秒に達したら攻撃が終了
    if (attackTimer_ >= easeTMax) {
        // 攻撃終了時には再びタイマーをリセット
        attackTimer_ = easeTMax;
        // 攻撃後の処理（攻撃後の遷移など）
        isNextAttack_ = true;
    }
}

void Player::UpAttack() {
    groupName = "UpAttack";
    damage = 1;
    knockBackValue = 0.5f;
    // 補間するための開始角度と終了角度を設定
    startAngle = globalVariables->GetVector3Value(groupName, "startAngle");
    endAngle = globalVariables->GetVector3Value(groupName, "endAngle");
    easeTMax = globalVariables->GetFloatValue(groupName, "easeTMax");
    startPosition = globalVariables->GetVector3Value(groupName, "startPosition");
    endPosition = globalVariables->GetVector3Value(groupName, "endPosition");
    startPosition.x = transform_.translation_.x;
    startPosition.y = transform_.translation_.y + startPosition.y;
    startPosition.z = transform_.translation_.z;
    endPosition.x = transform_.translation_.x;
    endPosition.z = transform_.translation_.z;
    startAngle.y = transform_.rotation_.y - startAngle.y; // 初期角度
    endAngle.y = transform_.rotation_.y - endAngle.y;     // 最終角度a
    // easeTMax = 1.0f;
    if (attackTimer_ == 0) {
        isAttack_ = true;
    }
    if (attackTimer_ < easeTMax) {
        attackTimer_ += Frame::DeltaTime(); // 時間を進める
    }

    // 0.5秒間で補間を適用
    R_arm_wt.rotation_ = EaseInOutQuad<Vector3>(startAngle, endAngle, attackTimer_, easeTMax);            // 0.5秒で補間
    transform_.translation_ = EaseInOutQuad<Vector3>(startPosition, endPosition, attackTimer_, easeTMax); // 0.5秒で補間
    transform_.rotation_ = EaseInOutQuad<Vector3>(startAngle, endAngle, attackTimer_, easeTMax);          // 0.5秒で補間

    // タイマーが0.5秒に達したら攻撃が終了
    if (attackTimer_ >= easeTMax) {
        // 攻撃終了時には再びタイマーをリセット
        attackTimer_ = easeTMax;
        // 攻撃後の処理（攻撃後の遷移など）
        isNextAttack_ = true;
    }
}

void Player::DownAttack() {
    groupName = "DownAttack";
    damage = 4;
    knockBackValue = 1.5f;
    // 補間するための開始角度と終了角度を設定
    startAngle = globalVariables->GetVector3Value(groupName, "startAngle");
    endAngle = globalVariables->GetVector3Value(groupName, "endAngle");
    easeTMax = globalVariables->GetFloatValue(groupName, "easeTMax");
    startPosition = globalVariables->GetVector3Value(groupName, "startPosition");
    endPosition = globalVariables->GetVector3Value(groupName, "endPosition");
    startPosition.x = transform_.translation_.x;
    startPosition.y = transform_.translation_.y;
    startPosition.z = transform_.translation_.z;
    endPosition.x = transform_.translation_.x;
    endPosition.z = transform_.translation_.z;
    startAngle.y = transform_.rotation_.y - startAngle.y; // 初期角度
    endAngle.y = transform_.rotation_.y - endAngle.y;     // 最終角度a
    // easeTMax = 1.0f;
    if (attackTimer_ == 0) {
        isAttack_ = true;
    }
    if (attackTimer_ < easeTMax) {
        attackTimer_ += Frame::DeltaTime(); // 時間を進める
    }

    // 0.5秒間で補間を適用
    R_arm_wt.rotation_ = EaseInOutQuad<Vector3>(startAngle, endAngle, attackTimer_, easeTMax);            // 0.5秒で補間
    transform_.translation_ = EaseInOutQuad<Vector3>(startPosition, endPosition, attackTimer_, easeTMax); // 0.5秒で補間
    transform_.rotation_ = EaseInOutQuad<Vector3>(startAngle, endAngle, attackTimer_, easeTMax);          // 0.5秒で補間

    // タイマーが0.5秒に達したら攻撃が終了
    if (attackTimer_ >= easeTMax) {
        if (!isNextAttack_) {
            isCrack_ = true;
        }
        isNextAttack_ = true;
        // 攻撃終了時には再びタイマーをリセット
        attackTimer_ = easeTMax;
        // 攻撃後の処理（攻撃後の遷移など）
    } else {
        isNextAttack_ = false;
    }
}

void Player::RowlingAttack() {
    groupName = "RowlingAttack";
    damage = 1;
    knockBackValue = 0.05f;
    // 補間するための開始角度と終了角度を設定
    startAngle = globalVariables->GetVector3Value(groupName, "startAngle");
    endAngle = globalVariables->GetVector3Value(groupName, "endAngle");
    easeTMax = globalVariables->GetFloatValue(groupName, "easeTMax");
    startAngle.y = transform_.rotation_.y - startAngle.y; // 初期角度
    endAngle.y = transform_.rotation_.y - endAngle.y;     // 最終角度a
    // easeTMax = 1.0f;
    if (attackTimer_ == 0) {
        isAttack_ = true;
    }
    if (attackTimer_ < easeTMax) {
        attackTimer_ += Frame::DeltaTime(); // 時間を進める
    }

    // 0.5秒間で補間を適用
    R_arm_wt.rotation_ = EaseInOutQuad<Vector3>(startAngle, endAngle, attackTimer_, easeTMax); // 0.5秒で補間
    transform_.rotation_ = EaseInSine<Vector3>(transform_.rotation_, {0.0f, transform_.rotation_.y, 0.0f}, attackTimer_, 0.1f);

    // タイマーが0.5秒に達したら攻撃が終了
    if (attackTimer_ >= easeTMax) {
        // 攻撃終了時には再びタイマーをリセット
        attackTimer_ = easeTMax;
        // 攻撃後の処理（攻撃後の遷移など）
        isNextAttack_ = true;
        behaviorRequest_ = Behavior::kRoot;
        comboStage_ = 0;
    }
}

#pragma endregion

#pragma region 動きの関数
void Player::Move() {
    const float diagonalSpeedFactor = 1.0f / sqrt(2.0f);
    // クールタイム処理
    if (dashCoolTime_ > 0.0f) {
        dashCoolTime_ -= Frame::DeltaTime(); // フレーム間隔を想定
    }

    // 移動ベクトルの初期化
    move = Vector3(0.0f, 0.0f, 0.0f);

    // カメラの向きに基づいて進行方向を決定
    if (camera_) {
        float cameraYaw = camera_->GetYaw(); // カメラのyawを取得

        // Wキーで進む方向を決定（カメラの向きに基づく）
        if (Input::GetInstance()->PushKey(DIK_W)) {
            move.x += std::sin(cameraYaw); // カメラの向きに基づいてX軸方向に進む
            move.z += std::cos(cameraYaw); // カメラの向きに基づいてZ軸方向に進む
        }
        // Sキーで後退
        if (Input::GetInstance()->PushKey(DIK_S)) {
            move.x -= std::sin(cameraYaw); // カメラの向きに基づいてX軸方向に後退
            move.z -= std::cos(cameraYaw); // カメラの向きに基づいてZ軸方向に後退
        }

        // Aキーで左移動（カメラの向きに基づく）
        if (Input::GetInstance()->PushKey(DIK_A)) {
            move.x -= std::cos(cameraYaw); // カメラの向きに基づいて左移動
            move.z += std::sin(cameraYaw); // カメラの向きに基づいてZ軸方向に進む
        }

        // Dキーで右移動（カメラの向きに基づく）
        if (Input::GetInstance()->PushKey(DIK_D)) {
            move.x += std::cos(cameraYaw); // カメラの向きに基づいて右移動
            move.z -= std::sin(cameraYaw); // カメラの向きに基づいてZ軸方向に進む
        }
    }

    if (!Input::GetInstance()->PushKey(DIK_D) && !Input::GetInstance()->PushKey(DIK_W) && !Input::GetInstance()->PushKey(DIK_A) && !Input::GetInstance()->PushKey(DIK_S)) {
        speed = {0.0f, 0.0f, 0.0f};
        move = {0.0f, 0.0f, 0.0f};
        dashSpeed_ = 0.0f;
    }

    // ダッシュ処理
    if ((Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_W) || Input::GetInstance()->PushKey(DIK_A) || Input::GetInstance()->PushKey(DIK_S)) && Input::GetInstance()->TriggerKey(DIK_LSHIFT) && dashCoolTime_ <= 0.0f) {
        Audio::GetInstance()->PlayWave(dashSE_, 0.2f);
        dashSpeed_ = kDashSpeed;
        dashCoolTime_ = 1.0f; // クールタイム設定
    }

    // 徐々にダッシュ速度を戻す
    dashSpeed_ *= kDashDecay;
    if (dashSpeed_ < kMoveSpeed) {
        dashSpeed_ = kMoveSpeed;
    }

    // 移動方向ベクトルがゼロでない場合に処理
    if (move.x != 0.0f || move.z != 0.0f) {
        if (move.x != 0.0f && move.z != 0.0f) {
            move.x *= diagonalSpeedFactor;
            move.z *= diagonalSpeedFactor;
        }
        speed = move * dashSpeed_;
        transform_.translation_ += speed; // ダッシュ速度を適用
    }
}

void Player::Rotation() {
    // 移動ベクトルがゼロでない場合の回転処理
    if (move.x != 0.0f || move.z != 0.0f) {
        float targetRotation = std::atan2(move.x, move.z); // 進行方向の角度計算
        const float rotationSpeed = 0.1f;                  // 補間速度

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
}

void Player::ArmFollow() {
    // 回転行列の計算
    float cosYaw = std::cos(-transform_.rotation_.y);
    float sinYaw = std::sin(-transform_.rotation_.y);

    // 右腕の位置を計算（時計回りの調整）
    R_arm_wt.translation_.x = transform_.translation_.x + (2.0f * cosYaw + 0.3f * sinYaw) + R_arm_offset.x;
    R_arm_wt.translation_.y = transform_.translation_.y - 0.3f + R_arm_offset.y;
    R_arm_wt.translation_.z = transform_.translation_.z + (2.0f * sinYaw - 0.3f * cosYaw) + R_arm_offset.z;

    // 左腕の位置を計算（時計回りの調整）
    L_arm_wt.translation_.x = transform_.translation_.x - (2.0f * cosYaw - 0.3f * sinYaw) + L_arm_offset.x;
    L_arm_wt.translation_.y = transform_.translation_.y - 0.3f + L_arm_offset.y;
    L_arm_wt.translation_.z = transform_.translation_.z - (2.0f * sinYaw + 0.3f * cosYaw) + L_arm_offset.z;
}

#pragma endregion

#pragma region エフェクト
void Player::AffterEffect() {
    afterImageEmitter_->SetPosition(transform_.translation_);

    // 最小と最大のfrequencyの値をローカル変数で設定
    const float minFrequency = 0.04f;
    const float maxFrequency = 0.1f;

    // speedが0.3fより大きいまたは-0.3fより小さい場合にパーティクルの間隔を設定
    if (speed.x > 0.3f || speed.y > 0.3f || speed.z > 0.3f ||
        speed.x < -0.3f || speed.y < -0.3f || speed.z < -0.3f) {
        // speedの大きさに応じて間隔を設定
        float speedMagnitude = std::max({std::abs(speed.x), std::abs(speed.y), std::abs(speed.z)});

        // 最小間隔と最大間隔をスケーリング
        float newFrequency = maxFrequency - (speedMagnitude - 0.3f) * (maxFrequency - minFrequency) / (1.0f - 0.3f);
        newFrequency = std::clamp(newFrequency, minFrequency, maxFrequency); // 間隔がminFrequencyからmaxFrequencyに収束するようにクランプ

        afterImageEmitter_->SetFrequency(newFrequency);
        afterImageEmitter_->SetCount(1); // 速度が速い場合にエフェクトを表示
        afterImageEmitter_->SetStartRotate(transform_.rotation_);
        afterImageEmitter_->SetEndRotate(transform_.rotation_);
    } else {
        afterImageEmitter_->SetCount(0); // 速度が遅い場合はエフェクトを非表示
    }
}
#pragma endregion

#pragma region 当たったときの呼び出し関数

Vector3 Player::GetCenterPosition() const {
    return BaseObject::GetCenterPosition();
}

Vector3 Player::GetCenterRotation() const {
    return BaseObject::GetCenterRotation();
}

void Player::OnCollision(Collider *other) {
    /*if (dynamic_cast<Enemy*>(other)) {
            ImGui::Begin("プレイヤー");
            ImGui::Text("OnCollision");
            ImGui::End();
    }*/
}

void Player::OnCollisionEnter(Collider *other) {
    if (dynamic_cast<Enemy *>(other) && invincibleTime == 0 && startCoolTime_ < 0.0f) {
        HP_--;
        invincibleTime = 1.0f;
        Audio::GetInstance()->PlayWave(damageSE_, 0.2f);
    }
}

#pragma endregion
