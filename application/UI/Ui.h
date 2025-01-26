#pragma once
#include"Sprite.h"
#include <memory>
#include"externals/nlohmann/json.hpp"
class UI
{
public:
	void Init();
	void Update();
	void Draw();
	void Debug();
	void SaveToJson();
	void LoadFromJson();
private:
	using json = nlohmann::json;
	std::unique_ptr<Sprite> explanation_;
	std::unique_ptr<Sprite> explanation2_;
	Vector2 spPos_;
	Vector2 spPos2_;
};

