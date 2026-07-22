#include "pch.h"
#include "CTurret.h"
#include "CTimeMgr.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"
#include "CSelectionMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CSCV.h"
#include "CUIMgr.h"
#include "CResourceMgr.h"
#include "CSoundMgr.h"
#include "CTurretBullet.h"

CTurret::CTurret()
{
}

CTurret::~CTurret()
{
}

void CTurret::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 128.f;
	m_tInfo.fCY = 128.f;
	//데미지, 공격 사거리 설정
	m_fAttackInterval = 1.f;
	m_fAttackTimer = m_fAttackInterval;
	m_fAttackRange = 200.f;

	lstrcpy(m_szGreenKey, L"TURRET");
	lstrcpy(m_szRedKey, L"TURRET");
	m_pFrameKey = L"TURRET_ANIM";
	m_eRender = RENDER_WORLD;
	m_tFrame.iStart = 0;
	m_tFrame.iFrame = 2;
	m_tFrame.iEnd = 34;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 50;

	m_eOriginalRace = eRaceType::RACE_TERRAN;
	m_eCurrentRace = eRaceType::RACE_TERRAN;
}

void CTurret::SetBuildingData()
{
	m_eType = eBuildingType::TURRET;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 500;
	m_fConstructDuration = 2.f;
	//타일 단위 크기
	m_iHeight = 2;
	m_iWidth = 2;
}

int CTurret::Update()
{
	int ret = CBuilding::Update();

	UpdateHotKeys();

	__super::Update_Rect();

	return ret;
}

void CTurret::Render(HDC hDC)
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
		RGB(0, 255, 0));
}

void CTurret::UpdateAttack()
{
	if (m_eState != eBuildingState::CONSTRUCT)
		return;
	//타겟 찾기
	if (!m_pTarget)
		m_pTarget = FindNearestEnemy();
	float dt = CTimeMgr::Get_Instance()->GetDT();
	//타겟 존재하면 attack!
	if (m_pTarget)
	{
		m_fAttackTimer += dt;
		if (m_fAttackTimer >= m_fAttackInterval)
		{
			FireBullet();
			m_fAttackTimer = 0.f;
		}
	}
}

void CTurret::FireBullet()
{
	if (!m_pTarget) //타겟 없으면 return
		return;

	//방향 계산 1번만!
	Vec2 myPos = Get_Pos();
	Vec2 targetPos = m_pTarget->Get_Pos();
	Vec2 vDir = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY};
	//거리 게산
	float dist = sqrtf(vDir.fX * vDir.fX + vDir.fY * vDir.fY);
	//사거리 체크
	if (dist > m_fAttackRange)
		return;
	//방향 정규화
	if (dist > 0.1f)
	{
		vDir.fX /= dist;
		vDir.fY /= dist;
	}
	//터렛 방향 설정 - 32방향
	m_vDir = vDir;
	m_tFrame.iFrame = DirTo16WayIndex(vDir) * 2 + 2; //2장은 버리고 시작!
	//공격 사운드 재생
	CSoundMgr::Get_Instance()->PlayEffect(L"Build/Turret.mp3", 0.2f);
	// 7. 투사체 생성
	CTurretBullet* pBullet = new CTurretBullet;
	pBullet->Initialize();
	pBullet->Set_Pos(myPos.fX, myPos.fY);
	pBullet->Set_Target(m_pTarget);
	pBullet->Set_Owner(this);
	pBullet->Set_Dir(vDir);
	pBullet->Set_Homing(true);
	pBullet->Set_Direction16(DirTo16WayIndex(vDir));  // 투사체는 16방향

	CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pBullet);

	/*
	//미사일 발사
	Vec2 targetPos = m_pTarget->Get_Pos();
	Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
	float fDT = CTimeMgr::Get_Instance()->GetDT();
	//타겟까지의 거리
	Vec2 diff = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY };
	float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
	//공격 사거리 체크
	if (dist <= m_fAttackRange)
	{
		//타겟 방향 보기
		if (dist > 0.1f)
		{
			m_vDir = { diff.fX / dist, diff.fY / dist };
		}
		m_tFrame.iFrame = DirTo16WayIndex(m_vDir);
		//히트스캔
		//m_pTarget->TakeDamage(m_iAttackDamage);
		//공격 사운드 재생
		//CSoundMgr::Get_Instance()->PlayEffect(L"Goliath/hkmissle.wav", 0.5f);
		CSoundMgr::Get_Instance()->PlayEffect(L"Build/Turret.mp3", 1.f);

		//현재 방향 가져오기 (16방향)
		int iCurrentDir = m_tFrame.iFrame;

		// 투사체 생성
		CTurretBullet* pBullet = new CTurretBullet;
		pBullet->Initialize();
		pBullet->Set_Pos(m_tInfo.fX, m_tInfo.fY);
		pBullet->Set_Target(m_pTarget);
		pBullet->Set_Owner(this);
		pBullet->Set_Homing(true); //타겟 따라가도록 설계
		// 방향 설정 (타겟 방향)
		Vec2 vMyPos = { m_tInfo.fX, m_tInfo.fY };
		Vec2 vTargetPos = { m_pTarget->Get_Info().fX, m_pTarget->Get_Info().fY };
		Vec2 vDir = { vTargetPos.fX - vMyPos.fX, vTargetPos.fY - vMyPos.fY };

		float fLength = sqrtf(vDir.fX * vDir.fX + vDir.fY * vDir.fY);
		if (fLength > 0.f)
		{
			vDir.fX /= fLength;
			vDir.fY /= fLength;
		}
		pBullet->Set_Dir(vDir);
		// 투사체도 16방향 인덱스 설정
		pBullet->Set_Direction16(iCurrentDir);
		// ObjMgr에 추가
		CObjMgr::Get_Instance()->Add_Object(OBJID::OBJ_PROJECTILE, pBullet);

		return;
	}
	*/
}

CObj* CTurret::FindNearestEnemy()
{
	list<CObj*> enemyList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_ENEMY);
	CObj* pNear = nullptr;
	float minDist = m_fAttackRange;
	for (auto* pObj : enemyList)
	{
		if (!pObj || pObj->IsDead())
			continue;
		//공중 유닛만 공격!
		if (pObj->GetLayer() != eUnitLayer::AIR)
			continue;
		Vec2 enemyPos = pObj->Get_Pos();
		Vec2 diff = { enemyPos.fX - m_tInfo.fX, enemyPos.fY - m_tInfo.fY };
		float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
		if (dist < minDist)
		{
			pNear = pObj;
			minDist = dist;
		}
	}
	return pNear;
}

void CTurret::UpdateAnimation()
{
	if (m_pTarget) //적이 존재할 경우 애니메이션 업데이트하지 않기 
		return;
	DWORD now = GetTickCount();
	// 생산 중: 애니메이션 진행 (2 ~ iEnd 반복)
	if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
	{
		m_tFrame.iFrame++;
		if (m_tFrame.iFrame >= m_tFrame.iEnd)
			m_tFrame.iFrame = 2;  // 1번 프레임으로 루프
		m_tFrame.dwTime = now;
	}
}

void CTurret::Release()
{
}

void CTurret::RenderSlot(HDC hDC, int slotIndex)
{
}

int CTurret::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CTurret::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);

	//0번 슬롯 SCV 생산
	//outSlot[0].commandID = eCommandID::SCV;
	//outSlot[0].iconKey = TEXT("ICON_SCV");
	//outSlot[0].hotkey = 'S';
	//outSlot[0].clickable = true;
	//outSlot[0].visible = true;
	//8번 : Cancle(Queue 취소)
	outSlot[8].commandID = eCommandID::CANCLE;
	outSlot[8].iconKey = TEXT("ICON_CANCLE");
	outSlot[8].hotkey = VK_ESCAPE;
	outSlot[8].clickable = true;
	outSlot[8].visible = true;
}

bool CTurret::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	//if (m_eState != eBuildingState::COMPLETE)
	//	return false;

	ResourceCost cost{};

	switch (command)
	{
	case eCommandID::SCV:
		cost.mineral = 50;
		cost.gas = 0;
		cost.supply = 1;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::SCV, 3.f, 3.f, 50, 0 });
		return true;
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

void CTurret::UpdateHotKeys()
{
	//SCV 유닛 하나만 선택되었을 경우 실행
	auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
	if (selected.size() != 1)
		return;
	//선택된 객체가 this인지 확인
	if (selected[0] != this)
		return;
	//슬롯 정보
	vector<CommandSlot> slots;
	this->CommandCardSlot(slots);
	//각 슬롯의 단축키 확인
	for (int i = 0; i < slots.size(); ++i)
	{
		if (!slots[i].visible || !slots[i].clickable)
			continue;
		//단축키가 눌렸는지 확인
		if (CInputMgr::Get_Instance()->KeyDownVK(slots[i].hotkey))
		{
			CUIMgr::Get_Instance()->SetButtonFeedback(i, true);
			//명령 실행
			CommandContext context{};
			this->ExecuteCommand(slots[i].commandID, context);
		}
	}
}

void CTurret::UpdateProduction()
{
	//건설 완료시 처리(사운드, 이펙트, 기능 오픈 포함 )
	if (m_queue.empty())
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_queue.front().remainTime -= dt;

	if (m_queue.front().remainTime <= 0.f)
	{
		eCommandID done = m_queue.front().command;
		m_queue.pop_front();
		ConstructComplete(done);
	}
}

void CTurret::ConstructComplete(eCommandID command)
{
	//인구수 올리기
	/*
	if (command == eCommandID::SCV)
	{
		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pSCV = CAbstractFactory<CSCV>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	}
	*/
}

void CTurret::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}
