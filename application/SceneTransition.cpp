#include "SceneTransition.h"
#include "TextureManager.h"
#include "algorithm"

SceneTransition::SceneTransition() {}

SceneTransition::~SceneTransition() {}

void SceneTransition::Initialize() {
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("white1x1.png", { 0,0 });
	sprite_->SetSize(Vector2(1280, 720));
	sprite_->SetColor(Vector4(0, 0, 0, 1));
	status_ = Status::None;
}

void SceneTransition::Update() {
	// フェード状態による分岐
	switch (status_) {
	case Status::None:
		// 何にもしない
		break;
	case Status::FadeIn:
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を小さくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - (counter_ / duration_), 0.0f, 1.0f)));
		break;
	case Status::FadeOut:
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	}
}

void SceneTransition::Draw() {
	if (status_ == Status::None) {
		return;
	}
	sprite_->Draw();
}

void SceneTransition::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void SceneTransition::Stop() { status_ = Status::None; }

bool SceneTransition::IsFinished() const {
	// フェード状態による分岐
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		}
		else {
			return false;
		}
	}

	return true;
}
