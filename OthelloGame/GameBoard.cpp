#include <DxLib.h>
#include "PieceST.h"
#include "GameBoard.h"
#include "GamePiece.h"
#include "Player.h"

GameBoard::GameBoard() : pieceSize(64),
defBoardCnt(8),
boardSize(pieceSize * defBoardCnt),
reverseInvCnt(20),
boardOffset((pieceSize * 2) + (pieceSize / 4), pieceSize)
{
	CommonBoard(Vector2(defBoardCnt, defBoardCnt));
}

GameBoard::GameBoard(Vector2 vec) : pieceSize(64),
defBoardCnt(8),
boardSize(pieceSize * defBoardCnt),
reverseInvCnt(20),
boardOffset((pieceSize * 2) + (pieceSize / 4), pieceSize)
{
	CommonBoard(vec);
}

GameBoard::~GameBoard()
{
}

bool GameBoard::CommonBoard(Vector2 vec)
{
	pPos	 = { 0,0 };
	drawFlag = false;
	invCnt	 = 0;
	// ピースを格納するためのサイズを取得している 
	pieceData.resize(vec.y * vec.x);
	data.resize(vec.y);
	for (unsigned int i = 0; i < data.size(); i++)
	{
		data[i] = &pieceData[i * vec.x];
	}
	return true;
}

auto GameBoard::AddObjList(piece_shared && objPtr)
{
	pieceList.push_back(objPtr);
	auto itr = pieceList.end();
	itr--;

	return itr;
}

// 画面サイズをクリックした座標に変換している 
Vector2 GameBoard::ChangeScrToPos(const Vector2& pPos)
{
	return Vector2((pPos.x - boardOffset.x), (pPos.y - boardOffset.y));
}

// データサイズを画面サイズに変換している
Vector2 GameBoard::ChangeTblToScr(const Vector2& pNum)
{
	return Vector2((pNum.x * pieceSize) + boardOffset.x, (pNum.y * pieceSize) + boardOffset.y);
}

void GameBoard::PutPieceField(void)
{
	// ピースが置ける位置の描画をしている 
	for (auto pNum : putPieceTbl)
	{
		DrawBox(ChangeTblToScr(pNum) + 1, ChangeTblToScr(pNum) + Vector2(pieceSize, pieceSize) - 1, 0xc8c800, true);
	}
}

Vector2 GameBoard::GetDataSize(void)
{
	return Vector2((pieceData.size() / data.size()), data.size());
}

void GameBoard::SetPiece(int x, int y, PIECE_ST state)
{
	// ピースの配置位置を設定している 
	pPos = ChangeTblToScr(Vector2(x, y));
	auto tmp = AddObjList(std::make_shared<GamePiece>(pPos, Vector2(0,0), state));
	data[y][x] = (*tmp);
}

bool GameBoard::SetPiece(const Vector2& vec, PIECE_ST id)
{
	bool rtnFlag = false;
	Vector2 pNum = ChangeScrToPos(vec);
	// クリックした位置に盤面上であるかの確認を行っている 
	if ((pNum >= Vector2(0, 0)) & (pNum < Vector2((data.size() * pieceSize), (data.size() * pieceSize))))
	{
		pNum /= pieceSize;
		pPos = ChangeTblToScr(pNum);
		if (data[pNum.y][pNum.x].expired())
		{
			rtnFlag = true;
			auto tmp = AddObjList(std::make_shared<GamePiece>(pPos, Vector2(0,0), id));
			data[pNum.y][pNum.x] = (*tmp);

			invCnt = reverseInvCnt * 4;		// 反転を行うまでの間隔を設定している
		}
	}
	return rtnFlag;
}

void GameBoard::SetPieceCnt(PutPiece piece)
{
	this->piece = piece;
	pieceList.clear();
}

PutPiece GameBoard::GetPieceCnt(void)
{
	return piece;
}

// リザルト画面でピースの並び替えを行うためのもの
void GameBoard::ResultPiece(PutPiece piece)
{
	pPos = {0,0};

	// 黒ピースの並び替えを行っている 
	for (int b = 0; b < piece.b; b++)
	{
		if (data[b / data.size()][b % data.size()].expired())
		{
			pPos = ChangeTblToScr(Vector2(b % data.size(), b / data.size()));
			auto tmp = AddObjList(std::make_shared<GamePiece>(pPos, Vector2(0,0), PIECE_ST::B));
			data[b / data.size()][b % data.size()] = (*tmp);
		}
	}
	// 白ピースの並び替えを行っている
	for (int w = piece.b; w < piece.b + piece.w; w++)
	{
		if (data[(w / data.size())][w % data.size()].expired())
		{
			pPos = ChangeTblToScr(Vector2(w % data.size(), w / data.size()));
			auto tmp = AddObjList(std::make_shared<GamePiece>(pPos, Vector2(0,0), PIECE_ST::W));
			data[(w / data.size())][w % data.size()] = (*tmp);
		}
	}
}

void GameBoard::SetReverse(const Vector2& vec, PIECE_ST id)
{
	Vector2 pNum	= ChangeScrToPos(vec);
	Vector2 rNum	= { 0,0 };
	int reverseCnt  = 0;

	pNum /= pieceSize;
	for (auto rPos : reverseTbl)
	{
		while (!data[pNum.y + rNum.y][pNum.x + rNum.x].expired())
		{
			rNum += rPos;
			// 置いたピースと違う色を見つけた場合、反転を行う
			if (data[pNum.y + rNum.y][pNum.x + rNum.x].lock()->GetState() != id)
			{
				reverseCnt++;
				data[pNum.y + rNum.y][pNum.x + rNum.x].lock()->SetState(id, reverseCnt * reverseInvCnt);
			}
			else
			{
				break;
			}
		}
		// コマの配置ができない間隔の設定
		if (invCnt < (reverseCnt * reverseInvCnt))
		{
			invCnt = reverseCnt * reverseInvCnt;
		}
		reverseCnt = 0;
		rNum = { 0,0 };
	}
	reverseTbl.clear();
}

bool GameBoard::CheckReverse(const Vector2& vec, PIECE_ST id)
{
	reverseTbl.clear();		// 置ける位置を登録する時に入ったリストの中身を削除している
	bool rtnFlag = false;
	Vector2 pNum = ChangeScrToPos(vec);

	// クリックした位置に盤面上であるかの確認を行っている
	if (pNum >= Vector2(0, 0) & pNum < Vector2(data.size() * pieceSize, data.size() * pieceSize))
	{
		pNum /= pieceSize;
		if (data[pNum.y][pNum.x].expired())
		{
			for (auto ckPos : pCheckTbl)
			{
				if (CheckReverse(ckPos, pNum, id))
				{
					rtnFlag = true;
				}
			}
		}
	}
	return rtnFlag;
}

bool GameBoard::CheckReverse(const Vector2& ckPos, const Vector2& pNum, PIECE_ST id)
{
	bool	rtnFlag = false;
	Vector2 ckNum = pNum;

	if (ckNum + ckPos >= Vector2(0, 0) & ckNum + ckPos < Vector2(data.size(), data.size()))
	{
		if (!data[pNum.y + ckPos.y][pNum.x + ckPos.x].expired())
		{
			if (data[pNum.y + ckPos.y][pNum.x + ckPos.x].lock()->GetState() != id)
			{
				while (!rtnFlag)
				{
					ckNum += ckPos;
					if (ckNum >= Vector2(0, 0) & ckNum < Vector2(data.size(), data.size()))
					{
						if (!data[ckNum.y][ckNum.x].expired())
						{
							if (data[ckNum.y][ckNum.x].lock()->GetState() == id)
							{
								rtnFlag = true;
								reverseTbl.push_back(ckPos);
							}
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	return rtnFlag;
}

void GameBoard::MakePutPieceField(PIECE_ST id)
{
	putPieceTbl.clear();
	for (unsigned int y = 0; y < data.size(); y++)
	{
		for (unsigned int x = 0; x < data.size(); x++)
		{
			if (data[y][x].expired())
			{
				for (auto ckPos : pCheckTbl)
				{
					if (CheckReverse(ckPos, Vector2(x, y), id))
					{
						// ピースの置ける位置の登録
						putPieceTbl.push_back(Vector2(x, y));
						break;
					}
				}
			}
		}
	}
}

bool GameBoard::CheckPutPiece(void)
{
	return (putPieceTbl.size() > 0);
}

void GameBoard::PutPieceClear(void)
{
	putPieceTbl.clear();
}

PIECE_ST GameBoard::CheckPutPieceST(int x, int y)
{
	// 盤面にピースが配置されていた時、ピースの色を取得するようにしている 
	if (!data[y][x].expired())
	{
		if (data[y][x].lock()->GetState() == PIECE_ST::W)
		{
			return PIECE_ST::W;
		}
		else if (data[y][x].lock()->GetState() == PIECE_ST::B)
		{
			return PIECE_ST::B;
		}
		else{}
	}
	return PIECE_ST::NON;
}

PutPiece GameBoard::PutPieceCnt(void)
{
	piece = { 0,0 };
	// ピースの色を取得して、それぞれの個数をカウントしている 
	for (int y = 0; y < data.size(); y++)
	{
		for (int x = 0; x < data.size(); x++)
		{
			if (CheckPutPieceST(x, y) == PIECE_ST::W)
			{
				piece.w++;
			}
			else if (CheckPutPieceST(x, y) == PIECE_ST::B)
			{
				piece.b++;
			}
			else {}
		}
	}

	return piece;
}


Vector2 GameBoard::GetPiecePos(PIECE_ST id)
{
	std::list<int> debugList;

	clock_t start, end;
	double time = 0;

	//// 処理時間の計測(debug用)	
	//for (int p = 0; p < 10; p++)
	//{
	//	start = clock();
	//	for (int i = 1; i < 100000; i++)
	//	{
	//		debugList.push_back(i);
	//	}
	//	end = clock();
	//	time += (double)(end - start);
	//	debugList.clear();

	//	/*if (time > 300)
	//	{
	//		break;
	//	}*/
	//}
	
	piece = PutPieceCnt();
	// ピースを置くとき、1ターン目の場合
	if ((piece.w + piece.b) < 6)
	{
		// CPUのピースをランダムで配置している
		if (putPieceTbl.size() > 0)
		{
			auto itr = putPieceTbl.begin();
			auto rand = GetRand(putPieceTbl.size() - 1);

			for (int i = 0; i < rand; i++)
			{
				itr++;
			}
			return ChangeTblToScr((*itr));
		}
	}
	else
	{
		// 配置するピースを決めている
		return ChoosePutPiece(putPieceTbl, id);
	}


	return {0,0};
}

Vector2 GameBoard::ChoosePutPiece(std::list<Vector2> pTbl, PIECE_ST id)
{
	
	std::list<int> pointList;	// メンバ変数にしてもいいかもしれない

	clock_t start, end = 0;
	int		point	   = 0;				// 評価点の保存用変数
	double	time	   = 0;
	bool    clockFlag  = false;
	

	start = clock();

	//for (Vector2 pNum : pTbl)
	//{
	//	if (!clockFlag)
	//	{
	//		for (Vector2 ckPos : pCheckTbl)
	//		{
	//			if (!clockFlag)
	//			{
	//				// 評価点の設定を行っている。
	//				point += DecidePoint(pNum, ckPos, id);
	//			}
	//		}
	//		end = clock();
	//		time = end - start;
	//		clockFlag = (time > 300 ? true : false);
	//		pointList.push_front(point);
	//		point = 0;
	//	}
	//}

	// 評価点の登録(debug用)
	for (int i = 0; i < putPieceTbl.size(); i++)
	{
		pointList.push_back(10 * i);
	}

	point = pointList.front();
	auto pointTbl = pointList.begin();
	auto itr = putPieceTbl.begin();
	int size = 0;

	for (int p = 1; p < pointList.size(); p++)
	{
		(*pointTbl++);
		if (point < (*pointTbl))
		{
			point = (*pointTbl);
			size = p;
		}
		
	}
	
	for (int i = 0; i < size; i++)
	{
		(*itr++);
	}

	return ChangeTblToScr((*itr));
}

// 評価点を決定するもの
int GameBoard::DecidePoint(Vector2 pNum, Vector2 ckPos, PIECE_ST id)
{
	Vector2 ckNum = pNum;
	int point	  = 0;
	bool rtnFlag  = false;

	if (!data[pNum.y + ckPos.y][pNum.x + ckPos.x].expired())
	{
		if (data[pNum.y + ckPos.y][pNum.x + ckPos.x].lock()->GetState() != id)
		{
			while (!rtnFlag)
			{
				ckNum += ckPos;
				if (!data[ckNum.y][ckNum.x].expired())
				{
					if (data[ckNum.y][ckNum.x].lock()->GetState() == id)
					{
						rtnFlag = true;
						reverseTbl.push_back(ckPos);
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	return 0;
}

bool GameBoard::InvFlag(void)
{
	if (invCnt < 0)
	{
		return true;
	}
	return false;
}

bool GameBoard::InvFlag(bool drawFlag)
{
	this->drawFlag = drawFlag;

	if (invCnt < 0)
	{
		this->drawFlag = false;
		return true;
	}
	else
	{
		invCnt--;
	}
	return false;
}

void GameBoard::Update(void)
{
	if (invCnt < 0)
	{
		return;
	}
	invCnt--;
}

void GameBoard::Draw()
{
	Vector2 sPos = { boardOffset.x, boardOffset.y };
	Vector2 ePos = { boardSize + boardOffset.x, boardSize + pieceSize };
	
	// 盤面の描画 
	DrawBox(sPos, ePos, 0x107010, true);

	sPos = { boardOffset.x, boardOffset.y };
	ePos = { boardSize + boardOffset.x, boardSize };

	// グリッドの描画 
	for (int y = 0; y <= defBoardCnt + 1; y++)
	{
		sPos.y = pieceSize * y + boardOffset.y;
		ePos.y = pieceSize * y + boardOffset.y;
		DrawLine(sPos, ePos, 0xaaaaaa, 2);
	}

	sPos = { boardOffset.x, boardOffset.y };
	ePos = { boardSize + boardOffset.x, boardSize + pieceSize };
	for (int x = 0; x <= defBoardCnt; x++)
	{
		sPos.x = (pieceSize * x) + boardOffset.x;
		ePos.x = (pieceSize * x) + boardOffset.x;
		DrawLine(sPos, ePos, 0xaaaaaa, 2);
	}

	if (InvFlag() | drawFlag)
	{
		// ピースの置ける位置の描画
		PutPieceField();

		if (drawFlag)
		{
			// 反転が行われるまでの間隔
			DrawBox(pPos, pPos + Vector2(pieceSize, pieceSize), 0x48d1cc, true);
		}
		
	}
	else
	{
		// ピースの置いた位置の描画
		DrawBox(pPos, pPos + Vector2(pieceSize, pieceSize), 0x48d1cc, true);
	}

	// ピースの描画 
	for (auto itr : pieceList)
	{
		(*itr).Update();
		(*itr).Draw();
	}
}

int DrawLine(Vector2 sPos, Vector2 ePos, unsigned int color, int thickNess)
{
	DxLib::DrawLine(sPos.x, sPos.y, ePos.x, ePos.y, color, thickNess);
	return 0;
}