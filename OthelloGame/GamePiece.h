#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include <list>
#include "Vector2.h"
#include "PieceState.h"

class GamePiece;

using piece_ptr = std::shared_ptr<GamePiece>;
using piece_list = std::list<piece_ptr>;

using pState_ptr = std::unique_ptr<PieceState>;
using pState_list = std::list<pState_ptr>;

typedef struct
{
	int b;
	int w;
}PutPiece;


class GamePiece
{
public:
	GamePiece();
	GamePiece(const Vector2& pos, const Vector2& drawOffset, PIECE_ST pState);		// drawOffsetを引数に追加した形にしておく
	~GamePiece();
	PIECE_ST GetState(void);
	void SetState(PIECE_ST pState);
	bool SetPos(const Vector2& pos);
	int  GetSize(void);
	bool SetDrawOffset(const Vector2& drawOffset);
	void Draw(void);
private:
	pState_list  pState;
	Vector2		 pos;
	Vector2		 drawOffset;
};

int DrawBox(Vector2 sPos, Vector2 ePos, unsigned int color, int fillFlag);

