#include "pch.h"
#include "CPylon.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include "CSelectionMgr.h"
#include "CCommandMgr.h"

CPylon::CPylon()
{
}

CPylon::~CPylon()
{
}

void CPylon::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 64.f;
	m_tInfo.fCY = 64.f;
	lstrcpy(m_szGreenKey, L"Pylon");
	lstrcpy(m_szRedKey, L"COMMANDCENTER_RED");
	lstrcpy(m_szConstructKey, L"COMMANDCENTER_CONSTRUCT");
	m_pFrameKey = L"Pylon";
	m_eRender = RENDER_WORLD;
	m_tFrame.iFrame = 0;
	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 1;
	m_tFrame.iCol = 0;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 100;

	m_eOriginalRace = eRaceType::RACE_PROTOSS;
	m_eCurrentRace = eRaceType::RACE_PROTOSS;
}

void CPylon::SetBuildingData()
{
	m_eType = eBuildingType::PYLON;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 500;
	m_iMaxShield = 500;
	m_iShield = m_iMaxShield;
	m_fConstructDuration = 2.f;
	//타일 단위 크기
	m_iHeight = 2;
	m_iWidth = 2;
}

int CPylon::Update()
{
	int ret = CBuilding::Update();

	__super::Update_Rect();

	return ret;
}

void CPylon::Render(HDC hDC)
{
	//고스트 모드일 경우 고스트 렌더가 되도록 설정
	if (m_bGhost)
	{
		CBuilding::Render(hDC);
		return;
	}
	if (m_bConstructing) //건설 중일 경우 BuildAnim Render!
	{
		CBuilding::Render(hDC);
		return;
	}
	int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
	int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

	int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
	int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

	//파일런
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

	int iScrX = m_tFrame.iStart * (int)m_tInfo.fCX;
	int iScrY = m_tFrame.iFrame * (int)m_tInfo.fCY;

	GdiTransparentBlt(hDC,
		iDrawX,
		iDrawY,
		(int)m_tInfo.fCX,
		(int)m_tInfo.fCY,
		hMemDC,
		iScrX,
		iScrY,
		(int)m_tInfo.fCX,		// 복사할 이미지의 가로 사이즈
		(int)m_tInfo.fCY,		// 복사할 이미지의 세로 사이즈
		RGB(255, 255, 255));
}

void CPylon::Release()
{
}

void CPylon::RenderPylonPower(HDC hDC)
{
	bool bShowPower = false;

	if (CCommandMgr::Get_Instance()->IsPlacing())
	{
		bShowPower = true;
	}
	else 
	{
		auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
		for (auto& pObj : selected)
		{
			if (pObj == this)
			{
				bShowPower = true;
				break;
			}
		}
	}

	if (!bShowPower)
		return;

	int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
	int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

	int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
	int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

	//파일런 경계
	CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(L"Pylon_Range");

	if (pPng)
	{
		int iWidth = pPng->Get_Width();
		int iHeight = pPng->Get_Height();

		pPng->Render_Alpha_Custom(hDC,
			(int)m_tInfo.fX - iScrollX - iWidth / 2,
			(int)m_tInfo.fY - iScrollY - iHeight / 2, iWidth, iHeight,
			30);
	}
}

void CPylon::RenderSlot(HDC hDC, int slotIndex)
{
}

int CPylon::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CPylon::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
}

bool CPylon::ExecuteCommand(eCommandID command, CommandContext& context)
{
	return false;
}

void CPylon::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}

