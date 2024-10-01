#include"d3dx12.h"
#include"MyGame.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<MyGame> game = nullptr;
	game = std::make_unique<MyGame>();

	game->Init();

	/// ---------ゲームループ---------
	while (true)
	{
		// 更新
		game->Update();

		if (game->IsEndRequest()) {
			break;
		}
		// 描画
		game->Draw();
	}
	/// --------ループ終了--------

	game->Finalize();

	return 0;
}
