#include "pch.h"
#include "CObj.h"

CObj::CObj() : m_pFrameKey(L""), m_iDrawID(0), m_eRender(RENDER_END), m_bSelected(false)
	,m_bSelectable(true), m_bDead(false), m_eTeamType(eTeamType::ALLY)
{
	ZeroMemory(&m_tInfo, sizeof(INFO));
	ZeroMemory(&m_tRect, sizeof(RECT));
	ZeroMemory(&m_tFrame, sizeof(FRAME));
}

CObj::~CObj()
{
}

RECT CObj::GetWorldRect()
{
	RECT rc{};

	rc.left = (LONG)(m_tInfo.fX - m_tInfo.fCX * 0.5f);
	rc.top = (LONG)(m_tInfo.fY - m_tInfo.fCY * 0.5f);
	rc.right = (LONG)(m_tInfo.fX + m_tInfo.fCX * 0.5f);
	rc.bottom = (LONG)(m_tInfo.fY + m_tInfo.fCY * 0.5f);

	return rc;
}

void CObj::Update_Rect()
{
	m_tRect.left = LONG(m_tInfo.fX - (m_tInfo.fCX / 2.f));
	m_tRect.top = LONG(m_tInfo.fY - (m_tInfo.fCY / 2.f));
	m_tRect.right = LONG(m_tInfo.fX + (m_tInfo.fCX / 2.f));
	m_tRect.bottom = LONG(m_tInfo.fY + (m_tInfo.fCY / 2.f));
}

void CObj::Move_Frame()
{
}
