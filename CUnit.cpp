#include "pch.h"
#include "CUnit.h"

CUnit::CUnit() : m_fSpeed(0.f), m_bSelected(false), m_bDead(false), m_iHP(0), m_iMaxHP(0),
	m_bActiveOrder(false)
{
	m_vDir = { 1.f, 1.f };
	m_eState = eUnitState::IDLE;
	ZeroMemory(&m_tFrame, sizeof(FRAME));
}

CUnit::~CUnit()
{
}

int CUnit::Update()
{
	if (m_bDead)
		return DEAD;
	//오더 시작
	if (!m_bActiveOrder && !m_OrderQ.empty())
	{
		Order& order = m_OrderQ.front();
		if (order.eType == eOrderType::MOVE)
			IssueMove(order.vTargetPos);
		m_bActiveOrder = true;
	}
	//실행
	bool bFinished = false;
	if (m_bActiveOrder)
	{
		if (m_eState == eUnitState::MOVE)
			bFinished = UpdateMove();
	}

	// 3) 완료 처리
	if (bFinished)
	{
		if (!m_OrderQ.empty()) m_OrderQ.pop_front();
		m_bActiveOrder = false;
		m_eState = eUnitState::IDLE;
	}

	__super::Update_Rect();

	return 0;
}

void CUnit::IssueMove(const Vec2& worldTarget)
{
	m_eState = eUnitState::MOVE;
	m_vecPath.clear();
	m_iPathIndex = 0;
	//직선 이후에 A*로 업그레이드 
	m_vecPath.push_back(worldTarget);
}

bool CUnit::UpdateMove()
{
	if (m_iPathIndex >= (int)m_vecPath.size())
		return true;

	Vec2 vTarget = m_vecPath[m_iPathIndex];
	Vec2 vPos{ m_tInfo.fX, m_tInfo.fY };

	Vec2 vDist{ vTarget.fX - vPos.fX, vTarget.fY - vPos.fY };
	float fDist = vDist.fX * vDist.fX + vDist.fY * vDist.fY;

	const float arrive = 2.0f;
	if (fDist <= arrive * arrive)
	{
		m_iPathIndex++;
		return (m_iPathIndex >= (int)m_vecPath.size());
	}

	float dist = sqrtf(fDist);
	vDist.fX /= dist; 
	vDist.fY /= dist;

	// dt가 없으면 일단 1.f로 두고, 나중에 TimeMgr 붙여도 됨
	float dt = 1.f;
	// dt = CTimeMgr::Get_Instance()->GetDeltaTime(); // 너 프로젝트에 있으면 이걸로 교체

	m_tInfo.fX += vDist.fX * m_fSpeed * dt;
	m_tInfo.fY += vDist.fY * m_fSpeed * dt;

	return false;
}

int CUnit::DirTo16WayIndex(Vec2& vDir)
{
	// (중요) 화면 좌표계가 y 아래로 증가면, 수학 좌표계로 맞추려면 -y를 쓰는 게 안전
	float ang = atan2(-vDir.fY, vDir.fX); // -PI ~ PI

	float step = PI / 8.f;        // 22.5도
	int idx = (int)floorf((ang + (PI / 16.f)) / step); // 가까운 방향 반올림
	idx = (idx % 16 + 16) % 16;   // 0~15
	return idx;
}

RECT CUnit::GetWorldRect() const
{
	RECT rc{};

	rc.left = (LONG)(m_tInfo.fX - m_tInfo.fCX * 0.5f);
	rc.top = (LONG)(m_tInfo.fY - m_tInfo.fCY * 0.5f);
	rc.right = (LONG)(m_tInfo.fX + m_tInfo.fCX * 0.5f);
	rc.bottom = (LONG)(m_tInfo.fY + m_tInfo.fCY * 0.5f);

	return rc;
}
