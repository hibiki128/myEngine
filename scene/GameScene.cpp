#include "GameScene.h"
#include "SceneManager.h"
#include "myEngine/Frame/Frame.h"
#include <LightGroup.h>
#include <line/DrawLine3D.h>
#include <random>

void GameScene::Finalize() {
}

void GameScene::Initialize() {

    audio_ = Audio::GetInstance();
    objCommon_ = Object3dCommon::GetInstance();
    spCommon_ = SpriteCommon::GetInstance();
    ptCommon_ = ParticleCommon::GetInstance();
    input_ = Input::GetInstance();

    debugCamera_ = std::make_unique<DebugCamera>();
    debugCamera_->Initialize(&vp_);

    /// ===================================================
    /// 生成
    /// ===================================================
    player_ = std::make_unique<Player>();
    enemyManager_ = std::make_unique<EnemyManager>();
    followCamera_ = std::make_unique<FollowCamera>();
    skyDome_ = std::make_unique<SkyDome>();
    ground_ = std::make_unique<Ground>();
    ui_ = std::make_unique<UI>();
    hitStop_ = std::make_unique<HitStop>();

    /// ===================================================
    /// 初期化
    /// ===================================================
    player_->Init("Player");
    enemyManager_->Init(player_->GetPosition(), player_.get());
    followCamera_->Init();
    skyDome_->Init("SkyDome");
    ground_->Init("Ground");
    ui_->Init();
    hitStop_->Initialize();

    /// ===================================================
    /// セット
    /// ===================================================

    followCamera_->SetTarget(&player_->GetWorldTransform());
    player_->SetCamera(followCamera_.get());
    player_->SetVp(&vp_);
}

void GameScene::Update() {
    // デバッグモード時の処理
#ifdef _DEBUG
    Debug();
#endif // _DEBUG

    // カメラ更新
    CameraUpdate();

    // シーン切り替え
    ChangeScene();
    if (!isFinish_) {
        if (hitStop_->IsActive()) {
            hitStop_->Update();
            return;
        }
        if (player_->IsHit()) {
            hitStop_->Start();
            player_->SetIsHit(false);
        }

        // 他のオブジェクトの更新処理
        player_->Update();
        enemyManager_->Update();
        enemyManager_->SetPosition(player_->GetCenterPosition());
        skyDome_->Update();
        ground_->Update();
    }
        ui_->Update();
}

void GameScene::Draw() {
    /// -------描画処理開始-------

    objCommon_->DrawCommonSetting();
    //-----3DObjectの描画開始-----

    player_->Draw(vp_);
    enemyManager_->Draw(vp_);

    //--------------------------

    /// Particleの描画準備
    ptCommon_->DrawCommonSetting();
    //------Particleの描画開始-------

    //-----------------------------

    /// Spriteの描画準備
    spCommon_->DrawCommonSetting();
    //-----Spriteの描画開始-----

    //------------------------

    //-----線描画-----
    DrawLine3D::GetInstance()->Draw(vp_);
    //---------------

    /// ----------------------------------

    /// -------描画処理終了-------
}

void GameScene::DrawForOffScreen() {
    /// -------描画処理開始-------

    objCommon_->DrawCommonSetting();
    //-----3DObjectの描画開始-----
    skyDome_->Draw(vp_);
    ground_->Draw(vp_);
    player_->DrawCrack(vp_);
    //--------------------------

    /// Particleの描画準備
    ptCommon_->DrawCommonSetting();
    //------Particleの描画開始-------
    player_->DrawParticle(vp_);
    enemyManager_->DrawParticle(vp_);
    //-----------------------------

    /// Spriteの描画準備
    spCommon_->DrawCommonSetting();
    //-----Spriteの描画開始-----
    ui_->Draw();
    //------------------------

    /// ----------------------------------

    /// -------描画処理終了-------
}

void GameScene::Debug() {
    ImGui::Begin("GameScene:Debug");

    // 最初のタブバー
    if (ImGui::BeginTabBar("1")) {
        debugCamera_->imgui();
        LightGroup::GetInstance()->imgui();
        ImGui::EndTabBar();
    }

    ImGui::End(); // ダイアログの終了
    ImGui::Begin("ObjData");
    player_->imgui();
    ImGui::End();
    // その他のデバッグ情報
    player_->Debug();
    followCamera_->imgui();
    ui_->Debug();
    enemyManager_->Debug();
    hitStop_->imgui();
}

void GameScene::CameraUpdate() {
    if (debugCamera_->GetActive()) {
        debugCamera_->Update();
    } else {
        followCamera_->Update();
        vp_.matWorld_ = followCamera_->GetViewProjection().matWorld_;
        vp_.matView_ = followCamera_->GetViewProjection().matView_;
        vp_.matProjection_ = followCamera_->GetViewProjection().matProjection_;
        // vp_.UpdateMatrix();
    }
}

void GameScene::ChangeScene() {
    if (enemyManager_->GetDeadCount() >= 10) {
        isFinish_ = true;
    }
    if (!player_->IsAlive()) {
        isFinish_ = true;
        ui_->SetTexture("game/gameOver.png");
    }
    if (isFinish_) {
        ui_->SetStart(true);
    }
    if (ui_->IsFinish()) {
        sceneManager_->NextSceneReservation("TITLE");
    }
}