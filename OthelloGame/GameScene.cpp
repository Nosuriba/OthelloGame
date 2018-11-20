#include <DxLib.h>
#include "GameScene.h"
#include "MouseCtl.h"
#include "TitleScene.h"
#include "PieceST.h"

#define SCREEN_SIZE_X (800)
#define SCREEN_SIZE_Y (600)

std::unique_ptr<GameScene, GameScene::GameSceneDeleter> GameScene::s_Instance(new GameScene());

GameScene::GameScene()
{
	SysInit();
}

GameScene::~GameScene()
{
	DxLib_End();
}

void GameScene::Run()
{
	gScene = std::make_unique<TitleScene>();
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		(*sysMouse).Update();
		gScene = (*gScene).Update(std::move(gScene), sysMouse);
	}
}

Vector2 GameScene::GetScreenSize(void)
{
	return Vector2(SCREEN_SIZE_X, SCREEN_SIZE_Y);
}

int GameScene::UpDate()
{
	return 0;
}

int GameScene::SysInit()
{
	// システムの初期化が終わった後、ゲームの初期化を行うようにしている 
	DxLib::SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 16);
	/*DxLib::SetWindowIconID();*/
	DxLib::ChangeWindowMode(true);
	DxLib::SetWindowText("1701310_北川 潤一 : OthelloGame");
	if (DxLib_Init() == -1)
	{
		return false;
	}
	DxLib::SetDrawScreen(DX_SCREEN_BACK);

	sysMouse = std::make_shared<MouseCtl>();
	(*sysMouse).SetPlType(PL_TYPE::MAN);

	return 0;
}

int GameScene::SysDestroy()
{
	return 0;
}

