#include "pch.h"
#include "CVulture.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CVBullet.h"
#include "CTimeMgr.h"
#include "CObjMgr.h"

CVulture::CVulture()
{
}

CVulture::~CVulture()
{
    Release();
}

void CVulture::Initialize()
{
    m_tInfo.fCX = 100.f; //벌쳐 사이즈
    m_tInfo.fCY = 100.f;
    m_iMaxHP = 250;
    m_iHP = m_iMaxHP;
    m_fSpeed = 300.f;

    //공격 변수 초기화 
    m_fAttackRange = 192.f;
    m_dwAttackCoolTime = 1000;
    m_dwLastAttack = 0;
    m_iAttackDamage = 25;
    m_fAttackSpeed = 1.f;
    //시야 범위 
    m_iSightRange = 14;

    m_pFrameKey = L"Vulture";

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::VULTURE;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 0;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;
}

int CVulture::Update()
{
    int iResult = CUnit::Update();

    if (iResult == DEAD)
        return DEAD;

    DWORD now = GetTickCount();

    switch (m_eState)
    {
    case eUnitState::IDLE:
        m_tFrame.iStart = 0;
        break;
    case eUnitState::MOVE:
        m_tFrame.iFrame = DirTo16WayIndex(m_vDir);
        break;
    case eUnitState::ATTACK:
        break;
    case eUnitState::DIE:
        break;
    default:
        break;
    }

    __super::Update_Rect();

    return NOEVENT;
}

void CVulture::Late_Update()
{
    //선택이 되었을 경우 마우스 방향의 애니메이션 재생
    if (!m_bSelected) return;
    //이동 중이면 마우스 방향 애니메이션 재생 멈추기
    if (m_eState != eUnitState::IDLE) return;

    Vec2 vWorldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    Vec2 vDir{ vWorldMouse.fX - m_tInfo.fX, vWorldMouse.fY - m_tInfo.fY };

    int iDir16 = DirTo16WayIndex(vDir);
    m_tFrame.iFrame = iDir16;
}

void CVulture::Render(HDC hDC)
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
        RGB(255, 0, 255));
}

void CVulture::Release()
{
}

bool CVulture::UpdateAttack(Order& order)
{
    //타겟이 죽었거나 사라진 경우
    if (!order.pTarget || order.pTarget->IsDead())
    {
        m_eState = eUnitState::IDLE;
        return true;
    }

    Vec2 targetPos = order.pTarget->Get_Pos();
    Vec2 myPos = Get_Pos();
    //타겟까지의 거리
    Vec2 diff = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
    // 공격 사거리 체크
    if (dist <= m_fAttackRange)
    {
        m_eState = eUnitState::ATTACK;

        // 타겟 방향 보기
        if (dist > 0.1f)
        {
            m_vDir = { diff.fX / dist, diff.fY / dist };
            m_tFrame.iFrame = DirTo16WayIndex(m_vDir);  // 16방향 회전
        }

        // 공격 쿨타임 체크
        DWORD now = GetTickCount();
        DWORD attackCoolTime = (DWORD)(1000.f / m_fAttackSpeed);
        if (now - m_dwLastAttack >= attackCoolTime)
        {
            // 히트스캔 - 즉시 데미지
            order.pTarget->TakeDamage(m_iAttackDamage);

            // 시각 효과용 투사체 발사
            Fire_Bullet(order.pTarget);

            m_dwLastAttack = now;
        }
        return false;
    }
    else
    {
        // 타겟이 사거리 내에 존재하지 않을 경우 이동
        m_eState = eUnitState::MOVE;
        Vec2 dir = { diff.fX / dist, diff.fY / dist };
        m_vDir = dir;

        m_tFrame.iFrame = DirTo16WayIndex(m_vDir);  // 이동 방향으로 회전

        float fDT = CTimeMgr::Get_Instance()->GetDT();
        m_tInfo.fX += dir.fX * fDT * m_fSpeed;
        m_tInfo.fY += dir.fY * fDT * m_fSpeed;
        return false;
    }
}

void CVulture::Fire_Bullet(CObj* pTarget)
{
    if (!pTarget)
        return;

    // 배틀크루저 현재 방향 가져오기 (16방향)
    int iCurrentDir = m_tFrame.iFrame;

    // 투사체 생성
    CVBullet* pBullet = new CVBullet;
    pBullet->Initialize();
    pBullet->Set_Pos(m_tInfo.fX, m_tInfo.fY);
    pBullet->Set_Target(pTarget);
    pBullet->Set_Owner(this);

    // 방향 설정 (타겟 방향)
    Vec2 vMyPos = { m_tInfo.fX, m_tInfo.fY };
    Vec2 vTargetPos = { pTarget->Get_Info().fX, pTarget->Get_Info().fY };
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
}
