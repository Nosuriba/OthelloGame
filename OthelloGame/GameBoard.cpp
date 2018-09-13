#include <DxLib.h>
#include "GameBoard.h"
#include "GamePiece.h"
#include "MouseCtl.h"

#define PIECE_SIZE		(64)
#define BOARD_OFFSET_X	(PIECE_SIZE * 2)
#define BOARD_OFFSET_Y	(PIECE_SIZE)
#define DEF_BOARD_CNT	(8)
/* 盤面のサイズが変わる可能性があるので、この設定に修正が入る可能性がある*/
#define BOARD_SIZE		(PIECE_SIZE * DEF_BOARD_CNT)

GameBoard::GameBoard()
{
	CommonBoard(Vector2(DEF_BOARD_CNT, DEF_BOARD_CNT));
}

GameBoard::GameBoard(Vector2 vec)
{
	CommonBoard(vec);
}

GameBoard::~GameBoard()
{
}

bool GameBoard::CommonBoard(Vector2 vec)
{
	
	pieceData.resize(vec.y * vec.x);
	data.resize(vec.y);
	for (unsigned int i = 0; i < data.size(); i++)
	{
		data[i] = &pieceData[i * vec.x];
	}

	return true;
}

auto GameBoard::AddObjList(piece_ptr && objPtr)
{
	pieceList.push_back(objPtr);
	auto itr = pieceList.end();
	itr--;

	return itr;
}

void GameBoard::Update(const MouseCtl& mouseCtl)
{
	/* ピースデータの情報を保存する場所を設定している */
	Vector2 mPos = { (mouseCtl.GetPoint().x - BOARD_OFFSET_X) / PIECE_SIZE, 
					 (mouseCtl.GetPoint().y - BOARD_OFFSET_Y) / PIECE_SIZE } ;

	if (mouseCtl.GetButton()[PUSH_NOW] & (~mouseCtl.GetButton()[PUSH_OLD] & MOUSE_INPUT_LEFT))
	{
		/* ピースの要素が見つからなかった場合に、*/
		if (((mPos.x >= 0) & (mPos.x < data.size() - 1)) & ((mPos.y >= 0) & (mPos.y < data.size())))
		{

			if (data[mPos.y][mPos.x].expired())
			{
				/*auto tmp = AddObjList(std::make_shared<GamePiece>(mPos, Vector2(BOARD_OFFSET_X, BOARD_OFFSET_Y)));
				data[mPos.y][mPos.x] = (*tmp);
				data[mPos.y][mPos.x].lock()->SetState(PIECE_W);*/
			}
			else
			{
				data[mPos.y][mPos.x].lock()->SetReverse();
			}
		}
	}
}

void GameBoard::Draw()
{
	Vector2 sPos = { BOARD_OFFSET_X, BOARD_OFFSET_Y };
	Vector2 ePos = { BOARD_SIZE + BOARD_OFFSET_X, BOARD_SIZE + PIECE_SIZE };

	/* 盤面の描画をしている */
	DrawBox(sPos, ePos, 0x104010, true);

	sPos = { BOARD_OFFSET_X, BOARD_OFFSET_Y };
	ePos = { BOARD_SIZE + BOARD_OFFSET_X, BOARD_SIZE };

	/* グリッドの描画をしている*/
	for (unsigned int y = 0; y <= DEF_BOARD_CNT + 1; y++)
	{
		sPos.y = PIECE_SIZE * y;
		ePos.y = PIECE_SIZE * y;
		DrawLine(sPos, ePos, 0xffffff, 1);
	}

	sPos = { BOARD_OFFSET_X, BOARD_OFFSET_Y };
	ePos = { BOARD_SIZE + BOARD_OFFSET_X, BOARD_SIZE + PIECE_SIZE};

	/* 線の位置修正を行っておく*/
	for (unsigned int x = 0; x <= DEF_BOARD_CNT + 1; x++)
	{
		sPos.x = PIECE_SIZE * x;
		ePos.x = PIECE_SIZE * x;
		DrawLine(sPos, ePos, 0xffffff, 1);
	}

	/*for (auto itr : pieceList)
	{
		itr->Draw();
	}
	*/
}

int DrawLine(Vector2 sPos, Vector2 ePos, unsigned int color, int thickNess)
{
	DxLib::DrawLine(sPos.x, sPos.y, ePos.x, ePos.y, color, thickNess);
	return 0;
}

int DrawBox(Vector2 sPos, Vector2 ePos, unsigned int color, int fillFlag)
{
	DxLib::DrawBox(sPos.x, sPos.y, ePos.x, ePos.y, color, fillFlag);
	return 0;
}
