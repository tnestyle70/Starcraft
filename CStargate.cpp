#include "pch.h"
#include "CStargate.h"
#include "CTimeMgr.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"
#include "CSelectionMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CSCV.h"
#include "CUIMgr.h"
#include "CSoundMgr.h"
#include "CFogMgr.h"
#include "CTileMgr.h"
#include "CCorsair.h"
#include "CScout.h"
#include "CCarrier.h"
#include "CArbiter.h"
#include "CEffect.h"

CStargate::CStargate()
{
}

CStargate::~CStargate()
{
}

void CStargate::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 128.f;
	m_tInfo.fCY = 160.f;
	lstrcpy(m_szGreenKey, L"Stargate");
	lstrcpy(m_szRedKey, L"COMMANDCENTER_RED");
	lstrcpy(m_szConstructKey, L"COMMANDCENTER_CONSTRUCT");
	m_pFrameKey = L"Stargate";
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

void CStargate::Rally()
{
	//Rally를 누른 시점의 마우스 위치를 저장
	m_vRallyPoint = CInputMgr::Get_Instance()->GetWorldMouse();
	m_bHasRallyPoint = true;
}

void CStargate::SetBuildingData()
{
	m_eType = eBuildingType::STARGATE;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 1000;
	m_iMaxShield = 1000;
	m_iShield = m_iMaxShield;
	m_fConstructDuration = 2.f;
	//타일 단위 크기
	m_iHeight = 3;
	m_iWidth = 3;
}

int CStargate::Update()
{
	int ret = CBuilding::Update();
	//건설이 완료되었을 경우에만 생산
	if (m_eState == eBuildingState::CONSTRUCT)
	{
		UpdateProduction();
	}

	UpdateHotKeys();

	if (m_eTeamType == eTeamType::ENEMY)
	{
		//프로토스 적 스폰
		SpawnProtossEnemy();
	}

	__super::Update_Rect();

	return ret;
}

void CStargate::Render(HDC hDC)
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

void CStargate::Release()
{
}

void CStargate::RenderSlot(HDC hDC, int slotIndex)
{
}

int CStargate::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CStargate::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);

	if (m_eState == eBuildingState::CONSTRUCTING)
		return;

	//0번 슬롯 커세어 생산
	outSlot[0].commandID = eCommandID::CORSAIR;
	outSlot[0].iconKey = TEXT("ICON_SCV");
	outSlot[0].hotkey = 'W';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//1번 슬롯 스카웃
	outSlot[1].commandID = eCommandID::SCOUT;
	outSlot[1].iconKey = TEXT("ICON_SCV");
	outSlot[1].hotkey = 'E';
	outSlot[1].clickable = true;
	outSlot[1].visible = true;
	outSlot[1].lock = false;
	//2번 슬롯 캐리어
	outSlot[2].commandID = eCommandID::CARRIER;
	outSlot[2].iconKey = TEXT("ICON_SCV");
	outSlot[2].hotkey = 'R';
	outSlot[2].clickable = true;
	outSlot[2].visible = true;
	outSlot[2].lock = false;
	//3번 슬롯 아비터
	outSlot[3].commandID = eCommandID::ARBITER;
	outSlot[3].iconKey = TEXT("ICON_SCV");
	outSlot[3].hotkey = 'S';
	outSlot[3].clickable = true;
	outSlot[3].visible = true;
	outSlot[3].lock = false;
	//5번 슬롯 RALLY
	outSlot[5].commandID = eCommandID::RALLY;
	outSlot[5].iconKey = TEXT("ICON_SCV");
	outSlot[5].hotkey = 'F';
	outSlot[5].clickable = true;
	outSlot[5].visible = true;
	outSlot[5].lock = false;
	//8번 : Cancle(Queue 취소)
	outSlot[8].commandID = eCommandID::CANCLE;
	outSlot[8].iconKey = TEXT("ICON_CANCLE");
	outSlot[8].hotkey = 'V';
	outSlot[8].clickable = true;
	outSlot[8].visible = true;
}

void CStargate::UpdateHotKeys()
{
	CBuilding::UpdateHotKeys();
}

bool CStargate::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	if (m_eState == eBuildingState::GHOST ||
		m_eState == eBuildingState::CONSTRUCTING)
		return false;

	ResourceCost cost{};

	bool bWarpGateReady = CObjMgr::Get_Instance()->IsWarpGateReady();

	switch (command)
	{
	case eCommandID::CORSAIR:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 1;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		m_queue.push_back({ eCommandID::CORSAIR, 1.f, 1.f, 50, 50 });
		return true;
	case eCommandID::SCOUT:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 1;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		m_queue.push_back({ eCommandID::SCOUT, 1.f, 1.f, 50, 50 });
		return true;
	case eCommandID::CARRIER:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 2;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::CARRIER, 1.f, 1.f, 50, 50 });
		return true;
	case eCommandID::ARBITER:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 2;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::ARBITER, 1.f, 1.f, 50, 50 });
		return true;
	case eCommandID::RALLY:
		Rally();
		break;
	case eCommandID::CANCLE:
		//생산 중인 큐 취소
		if (m_queue.empty())
		{
			return false;
		}
		m_queue.pop_back();
		//환불 정책
		return true;
		break;
	default:
		break;
	}
	return false;
}

void CStargate::UpdateProduction()
{
	//건설 완료시 처리(사운드, 이펙트, 기능 오픈 포함 )
	if (m_queue.empty())
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_queue.front().remainTime -= dt;

	if (m_queue.front().remainTime <= 0.f)
	{
		//생산 완료 후 인구수 증가
		CResourceMgr::Get_Instance()->AddSupply(m_queue.front().unitSupply);
		eCommandID done = m_queue.front().command;
		m_queue.pop_front();
		ProductionComplete(done);
	}
}

void CStargate::ProductionComplete(eCommandID command)
{
	if (command == eCommandID::CORSAIR)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Corsair/pcordy00.wav", 1.f);

		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pCorsair = CAbstractFactory<CCorsair>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pCorsair);
		if (m_bHasRallyPoint)
		{
			//랠리 포인트로 이동
			CCorsair* ppCorsair = dynamic_cast<CCorsair*>(pCorsair);
			if (ppCorsair)
			{
				Order rallyOrder;
				rallyOrder.eType = eOrderType::MOVE;
				rallyOrder.dst = m_vRallyPoint;
				ppCorsair->PushOrder(rallyOrder);
			}
		}
	}
	if (command == eCommandID::SCOUT)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Scout/PScRdy00.wav", 1.f);

		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pScout = CAbstractFactory<CScout>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pScout);
		if (m_bHasRallyPoint)
		{
			//랠리 포인트로 이동
			CScout* ppScout = dynamic_cast<CScout*>(pScout);
			if (ppScout)
			{
				Order rallyOrder;
				rallyOrder.eType = eOrderType::MOVE;
				rallyOrder.dst = m_vRallyPoint;
				ppScout->PushOrder(rallyOrder);
			}
		}
	}
	if (command == eCommandID::CARRIER)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Carrier/pcardy00.wav", 1.f);

		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pCarrier = CAbstractFactory<CCarrier>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pCarrier);
		if (m_bHasRallyPoint)
		{
			//랠리 포인트로 이동
			CCarrier* ppCarrier = dynamic_cast<CCarrier*>(pCarrier);
			if (ppCarrier)
			{
				Order rallyOrder;
				rallyOrder.eType = eOrderType::MOVE;
				rallyOrder.dst = m_vRallyPoint;
				ppCarrier->PushOrder(rallyOrder);
			}
		}
	}
	if (command == eCommandID::ARBITER)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Arbiter/pabRdy00.wav", 1.f);

		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pArbiter = CAbstractFactory<CArbiter>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pArbiter);
		if (m_bHasRallyPoint)
		{
			//랠리 포인트로 이동
			CArbiter* ppArbiter = dynamic_cast<CArbiter*>(pArbiter);
			if (ppArbiter)
			{
				Order rallyOrder;
				rallyOrder.eType = eOrderType::MOVE;
				rallyOrder.dst = m_vRallyPoint;
				ppArbiter->PushOrder(rallyOrder);
			}
		}
	}
}

void CStargate::SpawnProtossEnemy()
{
	if (m_eTeamType == eTeamType::ALLY)
		return;

	if (!CObjMgr::Get_Instance()->IsStartProtossRush())
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_fSpawnTimer += dt;
	if (m_fSpawnTimer >= m_fSpawnInterval)
	{
		Vec2 pos = Get_Pos();
		pos.fY -= 100.f;
		/*

		//스카웃 스폰
		//CSoundMgr::Get_Instance()->PlayEffect(L"BattleCruiser/BattleCruiserBirth.wav", 0.5f);
		CObj* pScout = CAbstractFactory<CScout>::Create(pos.fX, pos.fY);
		pScout->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pScout);
		//아군 진영으로 이동
		CScout* ppScout = dynamic_cast<CScout*>(pScout);
		if (ppScout)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppScout->PushOrder(rallyOrder);
		}
		//커세어 스폰
		pos.fX += 50.f;
		CObj* pCorsair = CAbstractFactory<CCorsair>::Create(pos.fX, pos.fY);
		pCorsair->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pCorsair);
		//아군 진영으로 이동
		CCorsair* ppCorsair = dynamic_cast<CCorsair*>(pCorsair);
		if (ppCorsair)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppCorsair->PushOrder(rallyOrder);
		}
		*/
		//스폰 타이머 초기화
		m_fSpawnTimer = 0.f;
		m_fSpawnInterval += 10.f;
	}
}

void CStargate::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}

void CStargate::UpdateDestroy()
{
	//타일 점유 해제
	ReleaseOccupy();
	//사운드 재생
	//CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingDeath.wav", 1.f);
	//타겟 이펙트 생성
	CObj* pEffect = CAbstractFactory<CEffect>::Create(
		m_tInfo.fX, m_tInfo.fY);
	pEffect->Initialize();
	CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
	if (pEffectObj)
	{
		pEffectObj->Set_Effect(L"Protoss_Building_Destroy_",
			14, 252, 200, eEffectType::COL_BASE, RGB(0, 0, 0), 0.05f, false,
			eEffectImageType::PNG);
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
	CObjMgr::Get_Instance()->IncreaseDestroyCount();
}
