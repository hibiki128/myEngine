#include <string>
#include "WorldTransform.h"  // Transformの定義があるヘッダーをインクルード
#include "ParticleManager.h" // ParticleManagerのインクルード
#include "ViewProjection.h"
#ifdef _DEBUG
#include "imgui.h" // ImGuiのインクルード
#endif // _DEBUG
#include "Object3d.h"

class ParticleEmitter {
public:
    // コンストラクタでメンバ変数を初期化
    ParticleEmitter();

    void Initialize(const std::string& name, const std::string& fileName);

    // 更新処理を行うUpdate関数
    void Update(const ViewProjection& vp_);

    void Draw();

    void DrawEmitter(const ViewProjection& vp_);

    void RenderImGui(); // ImGui用の関数を追加

private:
    // パーティクルを発生させるEmit関数
    void Emit();

    std::string name_;          // パーティクルの名前
    WorldTransform transform_;       // 位置や回転などのトランスフォーム
    float emitFrequency_;       // パーティクルの発生頻度
    int count_;                 // 一度に発生させるパーティクルの数
    float elapsedTime_;         // 経過時間

    // Emitに関連するメンバ変数を追加
    Vector3 velocityMin_;       // 速度の最小値
    Vector3 velocityMax_;       // 速度の最大値
    float lifeTimeMin_;         // ライフタイムの最小値
    float lifeTimeMax_;         // ライフタイムの最大値

    float deltaTime = 1.0f / 60.0f;

    bool isVisible;

    bool isBillBoard = true;

    std::unique_ptr<Object3d> emitterObj;
    std::unique_ptr<ParticleManager> Manager_;
};