#include "pch.h"
#include "CTile.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include <cwchar>

CTile::CTile() : m_iOption(0), m_iCost(0)
{
}

CTile::~CTile()
{
	Release();
}

void CTile::Initialize()
{
	m_tInfo.fCX = TILECX;
	m_tInfo.fCY = TILECY;

	m_pFrameKey = L"Tile";
}

int CTile::Update()
{
	__super::Update_Rect();

	return 0;
}

void CTile::Late_Update()
{
}

void CTile::Render(HDC hDC)
{
	int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
	int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

	HDC	hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

	BitBlt(hDC,		// 복사 받을 DC
		m_tRect.left + iScrollX,	// 복사 받을 LEFT
		m_tRect.top + iScrollY,				// 복사 받을 TOP
		(int)m_tInfo.fCX,			// 복사 받을 가로 길이
		(int)m_tInfo.fCY,			// 복사 받을 세로 길이
		hMemDC,		// 복사할 이미지 DC
		(int)m_tInfo.fCX * m_iDrawID,			// 이미지의 LEFT
		0,			// 이미지의 TOP
		SRCCOPY);	// 복사 방식
}

void CTile::Release()
{
}
