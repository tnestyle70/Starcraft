#include "pch.h"
#include "CPortraitMgr.h"
#include "CBmpMgr.h"
#include "CTimeMgr.h"

CPortraitMgr* CPortraitMgr::m_pInstance = nullptr;

void CPortraitMgr::Initialize()
{
	m_fAnimDuration = 0.1f;
	m_fAnimTimer = m_fAnimDuration;
	m_iCurrentFrame = 5;
	m_iEndFrame = 44;
}

void CPortraitMgr::Update()
{
	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_fAnimTimer += dt;
	if (m_fAnimTimer >= m_fAnimDuration)
	{
		if (!m_bReverse)
		{
			m_iCurrentFrame++;
			if (m_iCurrentFrame >= m_iEndFrame)
			{
				m_bReverse = true;
			}
		}
		else
		{
			m_iCurrentFrame--;
			if (m_iCurrentFrame <= 5)
			{
				m_bReverse = false;
			}
		}
		m_fAnimTimer = 0.f;
	}
}

void CPortraitMgr::RenderPortraitBMP(HDC hDC, eUnitType type)
{
	HDC hMemDC = nullptr;

	switch (type)
	{
	case eUnitType::NONE:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"ADVISOR_PORTRAIT");
		break;
	case eUnitType::SCV:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"SCV_PORTRAIT");
		break;
	case eUnitType::MARINE:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"MARINE_PORTRAIT");
		break;
	case eUnitType::MEDIC:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"MEDIC_PORTRAIT");
		break;
	case eUnitType::FIREBAT:
		break;
	case eUnitType::GHOST:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"GHOST_PORTRAIT");
		break;
	case eUnitType::VULTURE:
		break;
	case eUnitType::TANK:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"TANK_PORTRAIT");
		break;
	case eUnitType::SIEGE_TANK:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"TANK_PORTRAIT");
		break;
	case eUnitType::GOLIATH:
		break;
	case eUnitType::BATTLECRUISER:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"BATTLECRUISER_PORTRAIT");
		break;
	case eUnitType::ZERGLING:
		break;
	case eUnitType::HYDRALISK:
		break;
	case eUnitType::ULTRALISK:
		break;
	case eUnitType::MUTALISK:
		break;
	case eUnitType::OVERLOAD:
		break;
	case eUnitType::ZEALOT:
		break;
	default:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"MARINE_PORTRAIT");
		break;
	}

	if (!hMemDC)
		return;

	int iScrX = m_iWidth * m_iCurrentFrame;
	int iScrY = 0;

	BitBlt(hDC,
		m_iPortraitX,
		m_iPortraitY,
		m_iWidth,
		m_iHeight,
		hMemDC,
		iScrX,
		iScrY,
		SRCCOPY);  // 래스터 연산 코드
}

void CPortraitMgr::RenderPortraitBMPRow(HDC hDC, eUnitType type)
{
	//현재 프레임과 동일한 키 찾기
	TCHAR szKey[128];

	switch (type)
	{
	case eUnitType::FIREBAT:
		wsprintf(szKey, L"FireBat_Portrait_%d", m_iCurrentFrame);
		break;
	case eUnitType::VULTURE:
		wsprintf(szKey, L"Vulture_Portrait_%d", m_iCurrentFrame);
		break;
	case eUnitType::GOLIATH:
		wsprintf(szKey, L"Goliath_Portrait_%d", m_iCurrentFrame);
		break;
	default:
		break;
	}

	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Bmp_Anim(szKey);

	if (!hMemDC)
		return;

	int iScrX = 0;
	int iScrY = 0;

	BitBlt(hDC,
		m_iPortraitX,
		m_iPortraitY,
		m_iWidth,
		m_iHeight,
		hMemDC,
		iScrX,
		iScrY,
		SRCCOPY);  // 래스터 연산 코드
}

void CPortraitMgr::RenderPortraitPNG(HDC hDC, eUnitType type, eRaceType raceType)
{
	TCHAR szKey[256];

	if (raceType == eRaceType::RACE_PROTOSS)
	{
		wsprintf(szKey, L"Protoss_Portraits_%d", m_iCurrentFrame);
	}
	else
	{
		switch (type)
		{
		case eUnitType::NONE:
			wsprintf(szKey, L"Hive_Portrait_%d", m_iCurrentFrame);
			break;
		case eUnitType::ZERGLING:
			wsprintf(szKey, L"Zergling_Portrait_%d", m_iCurrentFrame);
			break;
		case eUnitType::HYDRALISK:
			wsprintf(szKey, L"Hydralisk_Portrait_%d", m_iCurrentFrame);
			break;
		case eUnitType::ULTRALISK:
			wsprintf(szKey, L"Ultralisk_Portrait_%d", m_iCurrentFrame);
			break;
		case eUnitType::MUTALISK:
			wsprintf(szKey, L"Mutalisk_Portrait_%d", m_iCurrentFrame);
			break;
		case eUnitType::OVERLOAD:
			wsprintf(szKey, L"Hive_Portrait_%d", m_iCurrentFrame);
			break;
		default:
			wsprintf(szKey, L"Hive_Portrait_%d", m_iCurrentFrame);
			break;
		}
	}

	CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szKey);
	if (pPng)
	{
		int iWidth = pPng->Get_Width();
		int iHeight = pPng->Get_Height();

		pPng->Render_Alpha(hDC,
			m_iPortraitX,
			m_iPortraitY,
			iWidth, iHeight, false);
	}
}
