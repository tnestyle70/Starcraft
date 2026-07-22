#include "pch.h"
#include "CShieldBattery.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include "CSelectionMgr.h"
#include "CCommandMgr.h"
#include "CObjMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"
#include "CTimeMgr.h"
#include "CSoundMgr.h"

CShieldBattery::CShieldBattery()
{
}

CShieldBattery::~CShieldBattery()
{
}

void CShieldBattery::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 96.f;
	m_tInfo.fCY = 64.f;
	lstrcpy(m_szGreenKey, L"Shield_Battery");
	lstrcpy(m_szRedKey, L"Shield_Battery");
	lstrcpy(m_szConstructKey, L"COMMANDCENTER_CONSTRUCT");
	m_pFrameKey = L"Shield_Battery";
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

void CShieldBattery::SetBuildingData()
{
	m_eType = eBuildingType::SHIELD_BATTERY;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 500;
	m_iMaxShield = 500;
	m_iShield = m_iMaxShield;
	m_fConstructDuration = 2.f;
	//타일 단위 크기
	m_iHeight = 3;
	m_iWidth = 3;
}

int CShieldBattery::Update()
{
	int ret = CBuilding::Update();

	UpdateRefill();

	__super::Update_Rect();

	return ret;
}

void CShieldBattery::Render(HDC hDC)
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

	// 이미지 가져오기
	CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(m_pFrameKey);

	if (pPng)
	{
		int iWidth = pPng->Get_Width();
		int iHeight = pPng->Get_Height();

		pPng->Render_Alpha(hDC,
			(int)m_tInfo.fX - iScrollX - iWidth / 2,
			(int)m_tInfo.fY - iScrollY - iHeight / 2, iWidth, iHeight, false);
	}
}

void CShieldBattery::Release()
{
}

void CShieldBattery::RenderPylonPower(HDC hDC)
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

void CShieldBattery::RenderSlot(HDC hDC, int slotIndex)
{
}

int CShieldBattery::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CShieldBattery::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
}

bool CShieldBattery::ExecuteCommand(eCommandID command, CommandContext& context)
{
	return false;
}

void CShieldBattery::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}

void CShieldBattery::UpdateRefill()
{
	float dt = CTimeMgr::Get_Instance()->GetDT();
	
	//주변 프로토스 유닛들 찾아서 쉴드 배터리를 채워주는 역할을 한다
	list<CObj*>& unitList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_UNIT);
	for (auto& pObj : unitList)
	{
		Vec2 targetPos = pObj->Get_Pos();
		Vec2 diff = { targetPos.fX - m_tInfo.fX, targetPos.fY - m_tInfo.fY };
		float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
		CUnit* pUnit = dynamic_cast<CUnit*>(pObj);

		//MaxShield가 0인 쉴드가 없는 유닛들은 탐색하지 않는다.
		if (pUnit->Get_MaxShield() == 0)
			continue;

		if (dist <= m_fShieldRange && pUnit) //거리 내에 유닛이 존재하고 쉴드가 파괴된 상태인 경우 치유
		{
			int iShield = pUnit->Get_Shield();
			int iMaxShield = pUnit->Get_MaxShield();

			if (iShield == 0 || iShield != iMaxShield)
			{
				m_fShieldTimer += dt;
				if (m_fShieldTimer >= m_fShieldInterval)
				{
					pUnit->RefillShield(m_iShieldRefill);
					//이펙트 생성
					CObj* pEffect = CAbstractFactory<CEffect>::Create(
						targetPos.fX, targetPos.fY);
					pEffect->Initialize();
					CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
					if (pEffectObj)
					{
						pEffectObj->Set_Effect(L"ShieldBattery_",
							21, 64, 64, eEffectType::COL_BASE, RGB(0, 0, 0), 0.1f, false,
							eEffectImageType::PNG);
					}
					CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
					//사운드 재생
					CSoundMgr::Get_Instance()->PlayEffect(L"Medic/MedicHeal.wav", 5.f);
					//타이머 초기화
					m_fShieldTimer = 0.f;
				}
			}
		}
	}
}

