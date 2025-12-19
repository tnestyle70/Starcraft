#include "pch.h"
#include "CEdit.h"
#include "CBmpMgr.h"
#include "CTileMgr.h"
#include "CScrollMgr.h"

CEdit::CEdit()
{
}

CEdit::~CEdit()
{
	Release();
}

void CEdit::Initialize()
{
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Map/FightSpirit.bmp", L"Stage");

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Tile/Tile.bmp", L"Tile");

	CTileMgr::Get_Instance()->Initialize();
}

int CEdit::Update()
{
	CTileMgr::Get_Instance()->Update();
	return 0;
}

void CEdit::Late_Update()
{
	Key_Input();

	CTileMgr::Get_Instance()->Late_Update();
}

void CEdit::Render(HDC hDC)
{
	HDC	hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Stage");

	BitBlt(hDC,		// 복사 받을 DC
		0,
		0,
		WINCX,
		WINCY,
		hMemDC,		// 복사할 이미지 DC
		0,			// 이미지의 LEFT
		0,			// 이미지의 TOP
		SRCCOPY);	// 복사 방식

	CTileMgr::Get_Instance()->Render(hDC);
}

void CEdit::Release()
{
}
void CEdit::Key_Input()
{
	if (GetAsyncKeyState(VK_LEFT))
		CScrollMgr::Get_Instance()->Set_ScrollX(5.f);

	if (GetAsyncKeyState(VK_RIGHT))
		CScrollMgr::Get_Instance()->Set_ScrollX(-5.f);

	if (GetAsyncKeyState(VK_UP))
		CScrollMgr::Get_Instance()->Set_ScrollY(5.f);

	if (GetAsyncKeyState(VK_DOWN))
		CScrollMgr::Get_Instance()->Set_ScrollY(-5.f);

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		POINT ptMouse{};
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		ptMouse.x -= (int)CScrollMgr::Get_Instance()->Get_ScrollX();
		ptMouse.y -= (int)CScrollMgr::Get_Instance()->Get_ScrollY();

		CTileMgr::Get_Instance()->Picking_Tile(ptMouse, 1, 0);
	}

	if (GetAsyncKeyState('S'))
	{
		CTileMgr::Get_Instance()->Save_Tile();
	}
}
