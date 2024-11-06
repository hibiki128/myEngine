#include "Player.h"
#include <algorithm>
#include <cassert>
#include <imgui.h>
#include"application/camera/RailCamera.h"

Player::~Player() {
	// bulletの解放
	for (playerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Initilaize(ViewProjection* viewProjection, const Vector3& position) {
	// 引数として受け取ったデータをメンバ変数に記録する
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize("debug/suzannu.obj");

	reticle_ = std::make_unique<Sprite>();
	reticle_->Initialize("Reticle.png", { 640.0f,360.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f });

	reticleObj_ = std::make_unique<Object3d>();
	reticleObj_->Initialize("debug/ICO.obj");

	spritePos = reticle_->GetPosition();

	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = { 0.3f,0.3f,0.3f };
	
	Reticle3Dwt_.Initialize();
	Reticle3Dwt_.scale_ = { 3.0f,3.0f,3.0f };

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void Player::Update() {

	// 弾の削除
	Bulletdelete();

	// 攻撃処理
	Attack();

	TramsformReticle();

	MoveAim();

	// 弾更新
	for (playerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 行列更新
	worldTransform_.UpdateMatrix();
}

void Player::Draw() {
	obj_->Draw(worldTransform_, *viewProjection_);
	reticleObj_->Draw(Reticle3Dwt_, *viewProjection_);
	// 弾描画
	for (playerBullet* bullet : bullets_) {
		bullet->Draw(*viewProjection_);
	}
}

void Player::DrawUI()
{
	reticle_->Draw();
}

void Player::imgui()
{
	// キャラクターの座標を画面表示する処理
	if (ImGui::BeginTabBar("player")) {
		if (ImGui::BeginTabItem("Player")) {
			ImGui::DragFloat3("position", &worldTransform_.translation_.x, 0.1f);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Reticle")) {
			ImGui::DragFloat3("position", &Reticle3Dwt_.translation_.x, 0.1f);
			spritePos = reticle_->GetPosition();
			ImGui::DragFloat2("2dpos", &spritePos.x, 0.1f);
			reticle_->SetPosition(spritePos);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("view")) {
			ImGui::DragFloat3("position", &viewProjection_->translation_.x, 0.1f);
			ImGui::DragFloat3("rotation", &viewProjection_->rotation_.x, 0.1f);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void Player::Attack() {
	if (input_->TriggerMouseButton(0)) {

		// 弾の速度
		const float kBulletSpeed = 0.5f;
		Vector3 velocity(0, 0, kBulletSpeed);

		// 自機から照準オブジェクトへのベクトル
		velocity = GetWorldReticlePosition() - GetWorldPosition();
		velocity = velocity.Normalize() * kBulletSpeed;

		// 弾を生成し、初期化
		playerBullet* newBullet = new playerBullet();
		newBullet->Initialize(GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

void Player::Bulletdelete() {
	// デスフラグの立った弾を削除
	bullets_.remove_if([](playerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});
}

Vector3 Player::GetWorldReticlePosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = Reticle3Dwt_.matWorld_.m[3][0];
	worldPos.y = Reticle3Dwt_.matWorld_.m[3][1];
	worldPos.z = Reticle3Dwt_.matWorld_.m[3][2];

	return worldPos;
}

void Player::OnCollision() {}

void Player::TramsformReticle()
{
	Vector3 positionReticle = GetWorldReticlePosition();

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewPortMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);

	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = viewProjection_->matView_ * viewProjection_->matProjection_ * matViewport;

	// ワールド->スクリーン座標変換(ここで3Dから2Dになる)
	positionReticle = Transformation(positionReticle, matViewProjectionViewport);

	// スプライトのレティクル座標に設定
	reticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
}

void Player::MoveAim()
{
	// マウス座標を取得
	POINT mousePosition;
	GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	// マウス位置に基づく2Dレティクルの位置設定
	Vector2 mousePos2D(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
	reticle_->SetPosition(mousePos2D);

	// マウスの2D位置を3D空間でのレティクル位置に変換
	Vector3 reticle3DPos = Vector3(mousePos2D.x, mousePos2D.y, 1.0f); // 1.0fは遠近効果調整用のZ値

	// ビューポート行列逆変換でスクリーン座標からワールド座標に戻す
	Matrix4x4 invMatViewport = Inverse(MakeViewPortMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1));
	reticle3DPos = Transformation(reticle3DPos, invMatViewport);

	// カメラの位置を考慮し、ビュー・プロジェクション逆変換で3Dレティクル位置取得
	Matrix4x4 invViewProjection = Inverse(viewProjection_->matView_ * viewProjection_->matProjection_);
	reticle3DPos = Transformation(reticle3DPos, invViewProjection);

	// 3Dレティクルのワールド座標を更新
	Reticle3Dwt_.translation_ = reticle3DPos;
	Reticle3Dwt_.UpdateMatrix();
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}
