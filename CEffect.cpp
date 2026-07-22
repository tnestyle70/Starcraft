#include "pch.h"
#include "CEffect.h"
#include "CTimeMgr.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"

CEffect::CEffect()
{
}

CEffect::~CEffect()
{
}

void CEffect::Initialize()
{
	m_eRender = RENDER_WORLD;
	m_iFrameCount = 0;
	m_iCurrentFrame = 0;
}

int CEffect::Update()
{
	if (m_bDead)
		return DEAD;

	m_fFrameTime += CTimeMgr::Get_Instance()->GetDT();
	if (m_fFrameTime >= m_fFrameDelay)
	{
		m_fFrameTime = 0.f;
		m_iCurrentFrame++;
		if (m_iCurrentFrame >= m_iFrameCount)
		{
			if (m_bLoop) //루프를 도는 애니메이션이면 프레임 0으로 초기화 이후 계속 재생
			{
				m_iCurrentFrame = 0;
			}
			else //루프 아니면 바로 소멸
			{
				Set_Dead();
				return DEAD;
			}
		}
	}

	__super::Update_Rect();

	return NOEVENT;
}

void CEffect::Late_Update()
{
	__super::Update_Rect();

	return;
}

void CEffect::Render(HDC hDC)
{
	int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
	int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

	int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
	int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

	if (m_eImageType == eEffectImageType::BMP)
	{
		int iSrcX = 0;
		int iSrcY = 0;

		switch (m_eEffectType)
		{
		case eEffectType::COL_BASE:
			//가로 방향
			iSrcX = m_iCurrentFrame * m_tInfo.fCX;
			iSrcY = 0;
			break;
		case eEffectType::ROW_BASE:
			//세로 방향
			iSrcX = 0;
			iSrcY = m_iCurrentFrame * m_tInfo.fCY;
			break;
		case eEffectType::DIRECTIONAL:
			//세로 방향
			iSrcX = m_iCurrentFrame * m_tInfo.fCY;
			iSrcY = m_iDirection * m_tInfo.fCY;
			break;
		default:
			break;
		}
		HDC hEffectDC = CBmpMgr::Get_Instance()->Find_Image(m_pEffectKey);
		GdiTransparentBlt(hDC, iDrawX, iDrawY,
			(int)m_tInfo.fCX, (int)m_tInfo.fCY,
			hEffectDC, iSrcX, iSrcY,
			(int)m_tInfo.fCX, (int)m_tInfo.fCY,
			m_rgbTransparent);
	}
	else
	{
		//상태에 따른 다른 키 생성
		TCHAR szKey[256];
		wsprintf(szKey, L"%s%d", m_pEffectKey, m_iCurrentFrame);
		CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szKey);
		if (pPng)
		{
			int iWidth = pPng->Get_Width();
			int iHeight = pPng->Get_Height();
			pPng->Render_Alpha(hDC,
				iDrawX,
				iDrawY,
				iWidth,
				iHeight, false);
		}
	}
}

void CEffect::Release()
{
}

void CEffect::Set_Effect(const TCHAR* pKey, int iFrameCount, float fCX, float fCY, 
	eEffectType type, COLORREF transparent, float fDelay, bool bLoop,
	eEffectImageType imageType)
{
	m_eImageType = imageType;
	m_pEffectKey = pKey;
	m_iFrameCount = iFrameCount;
	m_tInfo.fCX = fCX;
	m_tInfo.fCY = fCY;
	m_fFrameDelay = fDelay;
	m_bLoop = bLoop;
	m_eEffectType = type;
	m_rgbTransparent = transparent;
}

void CEffect::Set_DirectionalEffect(const TCHAR* pKey, int iDirectionCount, int iFrameCount, 
	float fCX, float fCY,
	int iDirection, COLORREF transparent, float fDelay, bool bLoop,
	eEffectImageType imageType)
{
	m_eImageType = imageType;
	m_pEffectKey = pKey;
	m_iFrameCount = iFrameCount;
	m_iDirectionCount = iDirectionCount;
	m_iDirection = iDirection;
	m_tInfo.fCX = fCX;
	m_tInfo.fCY = fCY;
	m_fFrameDelay = fDelay;
	m_bLoop = bLoop;
	m_eEffectType = eEffectType::DIRECTIONAL;
	m_rgbTransparent = transparent;
}
