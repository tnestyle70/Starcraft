#include "pch.h"
#include "CEdit.h"
#include "CBmpMgr.h"
#include "CTileMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CCursorMgr.h"

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

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MyFolder/Tile/Tile.bmp", L"Tile");

	CCursorMgr::Get_Instance()->Initialize();

	CTileMgr::Get_Instance()->Initialize();
	CTileMgr::Get_Instance()->Load_Tile();
}

int CEdit::Update()
{
	CScrollMgr::Get_Instance()->Update();

	CTileMgr::Get_Instance()->Update();

	// 커서 
	CCursorMgr::Get_Instance()->Update();

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

	float fScrX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrY = CScrollMgr::Get_Instance()->Get_ScrollY();

	BitBlt(hDC,		// 복사 받을 DC
		0,
		0,
		WINCX,
		WINCY,
		hMemDC,		// 복사할 이미지 DC
		fScrX,			// 이미지의 LEFT
		fScrY,			// 이미지의 TOP
		SRCCOPY);	// 복사 방식

	CTileMgr::Get_Instance()->RenderGrid(hDC, fScrX, fScrY);
	// 커서 
	CCursorMgr::Get_Instance()->Render(hDC);
}

void CEdit::Release()
{
}

void CEdit::Key_Input()
{
	if (CInputMgr::Get_Instance()->KeyPress(LEFT_MOUSE))
	{
		POINT ptMouse{};
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);
		//마우스 좌표 월드 좌표로 변환
		ptMouse.x += (int)CScrollMgr::Get_Instance()->Get_ScrollX();
		ptMouse.y += (int)CScrollMgr::Get_Instance()->Get_ScrollY();

		CTileMgr::Get_Instance()->Picking_Tile(ptMouse, 1, 1);
	}

	if (CInputMgr::Get_Instance()->KeyPress(RIGHT_MOUSE))
	{
		POINT ptMouse{};
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		ptMouse.x += (int)CScrollMgr::Get_Instance()->Get_ScrollX();
		ptMouse.y += (int)CScrollMgr::Get_Instance()->Get_ScrollY();

		CTileMgr::Get_Instance()->Picking_Tile(ptMouse, 0, 1);
	}

	if (CInputMgr::Get_Instance()->KeyPressVK('M'))
	{
		POINT ptMouse{};
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		ptMouse.x += (int)CScrollMgr::Get_Instance()->Get_ScrollX();
		ptMouse.y += (int)CScrollMgr::Get_Instance()->Get_ScrollY();

		CTileMgr::Get_Instance()->Picking_Tile(ptMouse, 2, 1);
	}
	if (CInputMgr::Get_Instance()->KeyPressVK('G'))
	{
		POINT ptMouse{};
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		ptMouse.x += (int)CScrollMgr::Get_Instance()->Get_ScrollX();
		ptMouse.y += (int)CScrollMgr::Get_Instance()->Get_ScrollY();

		CTileMgr::Get_Instance()->Picking_Tile(ptMouse, 3, 1);
	}
	if (GetAsyncKeyState('T'))
	{
		CTileMgr::Get_Instance()->Save_Tile();
	}
}
