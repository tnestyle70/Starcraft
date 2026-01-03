#include "pch.h"
#include "CProjectile.h"
#include "CScrollMgr.h"
#include "CTimeMgr.h"

CProjectile::CProjectile() : m_vDir(0.f, 0.f), m_fSpeed(0), m_iDamage(0),
	m_pTarget(nullptr), m_pOwner(nullptr), m_bHoming(false), m_iDirection16(0)
{
}

CProjectile::~CProjectile()
{
	Release();
}

void CProjectile::Initialize()
{
	m_tInfo.fCX = 128.f;
	m_tInfo.fCY = 128.f;
	m_eRender = RENDER_WORLD;
}

int CProjectile::Update()
{
	if (m_bDead)
		return DEAD;
	//유도 미사일의 경우 타겟 추적
	if (m_bHoming && m_pTarget && !m_pTarget->IsDead())
	{
		Vec2 targetPos = { m_pTarget->Get_Pos() };
		Vec2 myPos = Get_Pos();
		m_vDir.fX = targetPos.fX - myPos.fX;
		m_vDir.fY = targetPos.fY - myPos.fY;
		float len = sqrtf(m_vDir.fX * m_vDir.fX + m_vDir.fY * m_vDir.fY);
		if (len > 0.f)
		{
			m_vDir.fX /= len;
			m_vDir.fY /= len;
			m_iDirection16 = DirTo16WayIndex(m_vDir);
		}
	}
	//이동
	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_tInfo.fX += m_fSpeed * dt * m_vDir.fX;
	m_tInfo.fY += m_fSpeed * dt * m_vDir.fY;
	
	ResolveCollision();

	__super::Update_Rect();

	return 0;
}

void CProjectile::Late_Update()
{
}

void CProjectile::Render(HDC hDC)
{
}

void CProjectile::Release()
{
}

void CProjectile::ResolveCollision()
{
	if (m_tInfo.fX < -100.f || m_tInfo.fY < -100.f ||
		m_tInfo.fX > 10000.f || m_tInfo.fY > 10000.f)
	{
		m_bDead = true;
	}
}

int CProjectile::DirTo16WayIndex(Vec2& vDir)
{
	float ang = atan2(-vDir.fY, vDir.fX);
	float step = PI / 8.f;
	int idx = (int)floorf((ang + (PI / 16.f)) / step);
	idx = (idx % 16 + 16) % 16;
	return idx;
}
