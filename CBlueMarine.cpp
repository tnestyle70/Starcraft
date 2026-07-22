#include "pch.h"
#include "CBlueMarine.h"
#include "CObjMgr.h"
#include "CScrollMgr.h"
#include "Define.h"
#include "CBmpMgr.h"
#include <algorithm>

CBlueMarine::CBlueMarine()
	: m_pCurrentTarget(nullptr)
	, m_fAIUpdateTimer(0.f)
	, m_fAIUpdateInterval(0.5f)	// 0.5초마다 AI 업데이트
{
}

CBlueMarine::~CBlueMarine()
{
}

void CBlueMarine::Initialize()
{
	m_tInfo.fCX = 50.f; //마린 한 칸 크기
	m_tInfo.fCY = 50.f;
	m_iMaxHP = 100;
	m_iHP = m_iMaxHP;
	m_fSpeed = 100.f;
	//공격 관련 변수
	m_iAttackDamage = 6;
	m_fAttackRange = 120.f;
	m_fAttackSpeed = 1.0f;
	//공격 애니메이션 프레임 
	m_iAttackFrameStart = 11;
	m_iAttackFrameEnd = 14;

	m_pFrameKey = L"BLUE_MARINE";

	m_eOriginalRace = eRaceType::RACE_TERRAN;
	m_eCurrentRace = eRaceType::RACE_TERRAN;

	m_eRender = RENDER_WORLD;
	m_eState = eUnitState::IDLE;
	m_eType = eUnitType::MARINE;
	m_tFrame.iFrame = 0;
	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 7;
	m_tFrame.iCol = 0;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 100;

	// AI 초기화
	m_pCurrentTarget = nullptr;
	m_fAIUpdateTimer = 0.f;
}

int CBlueMarine::Update()
{
	int iResult = CUnit::Update();

	if (iResult == DEAD)
		m_eState = eUnitState::DIE;

	// AI 업데이트
	UpdateAI();

	DWORD now = GetTickCount();

	switch (m_eState)
	{
	case eUnitState::IDLE:
		m_tFrame.iStart = 0;
		break;
	case eUnitState::MOVE:
		m_tFrame.iFrame = DirTo16WayIndex(m_vDir);

		if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
		{
			m_tFrame.iStart++;
			if (m_tFrame.iStart > m_tFrame.iEnd)
				m_tFrame.iStart = 0;
			m_tFrame.dwTime = now;
		}
		break;
	case eUnitState::ATTACK:
		//방향에 따른 행 설정
		m_tFrame.iFrame = DirTo16WayIndex(m_vDir);
		//애니메이션 진행
		if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
		{
			//첫 진입시 프레임 설정
			if (m_tFrame.iStart < m_iAttackFrameStart ||
				m_tFrame.iStart > m_iAttackFrameEnd)
			{
				m_tFrame.iStart = m_iAttackFrameStart;
			}
			else
			{
				m_tFrame.iStart++;
				if (m_tFrame.iStart > m_iAttackFrameEnd)
				{
					m_tFrame.iStart = m_iAttackFrameStart;  // 반복
				}
			}
			m_tFrame.dwTime = now;
		}
		break;
	case eUnitState::DIE:
		UpdateDead();
		return DEAD;
	default:
		break;
	}

	__super::Update_Rect();

	return NOEVENT;
}

void CBlueMarine::Late_Update()
{
	CMarine::Late_Update();
}

void CBlueMarine::Render(HDC hDC)
{
	//전장의 안개 
	CUnit::Render(hDC);

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
		RGB(255, 255, 0));

	// AI 디버그 정보 렌더링 (선택사항)
#ifdef _DEBUG
	if (m_pCurrentTarget && !m_pCurrentTarget->Is_Dead())
	{
		// 현재 타겟까지 선 그리기
		int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
		int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

		POINT startPt = { (LONG)(m_tInfo.fX + iScrollX), (LONG)(m_tInfo.fY + iScrollY) };
		POINT endPt = { (LONG)(m_pCurrentTarget->Get_Info().fX + iScrollX),
						(LONG)(m_pCurrentTarget->Get_Info().fY + iScrollY) };

		HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

		MoveToEx(hDC, startPt.x, startPt.y, nullptr);
		LineTo(hDC, endPt.x, endPt.y);

		SelectObject(hDC, hOldPen);
		DeleteObject(hPen);
	}
#endif
}

void CBlueMarine::Release()
{
	CMarine::Release();
}

void CBlueMarine::CommandCardSlot(vector<CommandSlot>& outSlot)
{
	return;
	//CUnit::CommandCardSlot(outSlot);
	////7번 : 시즈 모드로 변경0
	//outSlot[6].commandID = eCommandID::STEAMPACK;
	//outSlot[6].iconKey = TEXT("ICON_STEAMPACK");
	//outSlot[6].hotkey = 'X';
	//outSlot[6].clickable = true;
	//outSlot[6].visible = true;
}

void CBlueMarine::UpdateAI()
{
	// AI 업데이트 주기 체크 (매 프레임마다 하지 않고 0.5초마다)
	m_fAIUpdateTimer += 0.016f; // 약 60fps 기준
	if (m_fAIUpdateTimer < m_fAIUpdateInterval)
		return;

	m_fAIUpdateTimer = 0.f;

	// 현재 명령 실행 중이면 AI 개입 안함
	if (HasActiveOrder())
		return;

	// 현재 타겟이 유효한지 체크
	if (m_pCurrentTarget)
	{
		if (m_pCurrentTarget->Is_Dead())
		{
			m_pCurrentTarget = nullptr;
		}
	}

	// 타겟이 없거나 무효화됐으면 새로운 타겟 찾기
	if (!m_pCurrentTarget)
	{
		m_pCurrentTarget = FindNearestEnemy();
	}

	// 타겟이 있으면 공격 또는 추격
	if (m_pCurrentTarget)
	{
		if (IsInAttackRange(m_pCurrentTarget))
		{
			// 사거리 내 - 공격
			IssueAutoAttack(m_pCurrentTarget);
		}
		else
		{
			// 사거리 밖 - 추격
			IssueAutoMove(m_pCurrentTarget);
		}
	}
}

CObj* CBlueMarine::FindNearestEnemy()
{
	float fMinDistance = FLT_MAX;
	CObj* pNearestEnemy = nullptr;

	// 탐지 범위 (사거리 + 추가 탐지 범위)
	float fDetectionRange = m_fAttackRange; // 사거리 5 + 300 = 약 305 범위

	// 모든 유닛 검사
	auto& unitList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_UNIT);
	for (auto& pObj : unitList)
	{
		// 죽은 유닛 제외
		if (pObj->Is_Dead())
			continue;

		// 같은 팀 제외
		//if (pObj->Get_Team() == m_eTeam)
		//	continue;

		// 거리 계산
		float fDX = pObj->Get_Info().fX - m_tInfo.fX;
		float fDY = pObj->Get_Info().fY - m_tInfo.fY;
		float fDistance = sqrtf(fDX * fDX + fDY * fDY);

		// 탐지 범위 밖이면 제외
		if (fDistance > fDetectionRange)
			continue;

		// 가장 가까운 적 갱신
		if (fDistance < fMinDistance)
		{
			fMinDistance = fDistance;
			pNearestEnemy = pObj;
		}
	}

	return pNearestEnemy;
}

bool CBlueMarine::IsInAttackRange(CObj* pTarget)
{
	if (!pTarget || pTarget->Is_Dead())
		return false;

	float fDX = pTarget->Get_Info().fX - m_tInfo.fX;
	float fDY = pTarget->Get_Info().fY - m_tInfo.fY;
	float fDistance = sqrtf(fDX * fDX + fDY * fDY);

	// 사거리 내 체크 (약간의 여유 추가)
	return fDistance <= (m_fAttackRange);
}

bool CBlueMarine::HasActiveOrder()
{
	// 명령큐가 비어있지 않으면 명령 실행 중
	return !m_OrderQ.empty();
}

void CBlueMarine::IssueAutoAttack(CObj* pTarget)
{
	if (!pTarget || pTarget->Is_Dead())
		return;

	// 기존 명령 클리어
	m_OrderQ.clear();

	// 공격 명령 추가
	Order attackOrder;
	attackOrder.eType = eOrderType::ATTACK;
	attackOrder.pTarget = pTarget;
	attackOrder.dst = { pTarget->Get_Info().fX, pTarget->Get_Info().fY };

	m_OrderQ.push_back(attackOrder);
}

void CBlueMarine::IssueAutoMove(CObj* pTarget)
{
	if (!pTarget || pTarget->Is_Dead())
		return;

	// 기존 명령 클리어
	m_OrderQ.clear();

	// 이동 명령 추가 (타겟 위치로)
	Order moveOrder;
	moveOrder.eType = eOrderType::MOVE;
	moveOrder.dst = { pTarget->Get_Info().fX, pTarget->Get_Info().fY };

	m_OrderQ.push_back(moveOrder);
}
