#include "pch.h"
#include "CGoliath.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CUIMgr.h"
#include "CSelectionMgr.h"
#include "CTimeMgr.h"
#include "CGBullet.h"
#include "CObjMgr.h"

CGoliath::CGoliath()
{
}

CGoliath::~CGoliath()
{
    Release();
}

void CGoliath::Initialize()
{
    m_tInfo.fCX = 76.f; //골리앗 한 칸 크기
    m_tInfo.fCY = 76.f;
    m_iMaxHP = 125;
    m_iHP = m_iMaxHP;
    m_fSpeed = 200.f;
    //공격 변수 초기화
    m_iAttackDamage = 20;
    m_fAttackRange = 7.f * TILECX;
    m_fAttackSpeed = 1.f;

    m_pFrameKey = L"Goliath_Body";
    m_pHeadKey = L"Goliath_Head";

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::GOLIATH;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 15;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;
    //포탑 초기화
    m_vHeadDir = { 0.f, -1.f }; //위쪽 방향
    m_iHeadFrame = 0;
    m_bFiring = false;
    m_iFireFrame = 0;
}

int CGoliath::Update()
{
    int iResult = CUnit::Update();

    if (iResult == DEAD)
        return DEAD;

    UpdateBody();
    UpdateHead();

    __super::Update_Rect();

    return NOEVENT;
}

void CGoliath::Late_Update()
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

void CGoliath::UpdateBody()
{
    //몸체는 이동 방향 따라가기
    switch (m_eState)
    {
    case eUnitState::IDLE:
        m_tFrame.iStart = 0;
        break;
    case eUnitState::MOVE:
        m_tFrame.iFrame = DirTo16WayIndex(m_vDir);
        break;
    case eUnitState::ATTACK:
        //공격 중에도 몸체는 이동 방향 유지
        break;
    case eUnitState::DIE:
        break;
    default:
        break;
    }
}

void CGoliath::UpdateHead()
{
    DWORD now = GetTickCount();

    if (m_bSelected && m_eState == eUnitState::IDLE)
    {
        Vec2 worldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
        m_vHeadDir.fX = worldMouse.fX - m_tInfo.fX;
        m_vHeadDir.fY = worldMouse.fY - m_tInfo.fY;
        //정규화
        float len = sqrtf(m_vHeadDir.fX * m_vHeadDir.fX + m_vHeadDir.fY * m_vHeadDir.fY);
        if (len > 0.f)
        {
            m_vHeadDir.fX /= len;
            m_vHeadDir.fY /= len;
        }
        m_iHeadFrame = DirTo16WayIndex(m_vHeadDir);
    }
    else if (m_eState == eUnitState::MOVE)
    {
        m_vHeadDir = m_vDir;
        m_iHeadFrame = DirTo16WayIndex(m_vHeadDir);
    }
    else if (m_eState == eUnitState::ATTACK) //타겟 방향으로 머리 회전
    {
        if (m_OrderQ.front().pTarget)
        {
            Vec2 targetPos = m_OrderQ.front().pTarget->Get_Pos();
            m_vHeadDir.fX = targetPos.fX - m_tInfo.fX;
            m_vHeadDir.fY = targetPos.fY - m_tInfo.fY;
            //정규화
            float len = sqrtf(m_vHeadDir.fX * m_vHeadDir.fX + m_vHeadDir.fY * m_vHeadDir.fY);
            if (len > 0.f)
            {
                m_vHeadDir.fX /= len;
                m_vHeadDir.fY /= len;
            }
            m_iHeadFrame = DirTo16WayIndex(m_vHeadDir);
        }
        DWORD attackCoolTime = (DWORD)(1000.f / m_fAttackSpeed);
        DWORD timeSinceLastAttack = now - m_dwLastAttack;

        // 공격 직후에 발사 애니메이션 시작
        if (timeSinceLastAttack < 100)  // 공격 후 100ms 동안만 발사 애니메이션 표시
        {
            m_bFiring = true;
            // col 10으로 이동 (발사 프레임)
            m_iFireFrame = 10;
        }
        else
        {
            m_bFiring = false;
            m_iFireFrame = 0;  // col 0 = 일반 상태
        }
    }
}

void CGoliath::RenderBody(HDC hDC)
{
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

void CGoliath::RenderHead(HDC hDC)
{

    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    HDC hHeadDC = CBmpMgr::Get_Instance()->Find_Image(m_pHeadKey);

    int iSrcX = m_bFiring ? (m_iFireFrame * (int)m_tInfo.fCX) : 0;  // 발사중이면 애니메이션
    int iSrcY = m_iHeadFrame * (int)m_tInfo.fCY;

    GdiTransparentBlt(hDC,
        iDrawX,
        iDrawY,
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        hHeadDC,
        iSrcX,
        iSrcY,
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        RGB(0, 255, 0));
}

void CGoliath::Render(HDC hDC)
{
    //전장의 안개 
    CUnit::Render(hDC);

    RenderBody(hDC);
    RenderHead(hDC);
}

void CGoliath::Release()
{
}

bool CGoliath::UpdateAttack(Order& order)
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
        /*
        if (dist > 0.1f)
        {
            m_vDir = { diff.fX / dist, diff.fY / dist };
            m_tFrame.iFrame = DirTo16WayIndex(m_vDir);  // 16방향 회전
        }
        */
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

void CGoliath::Fire_Bullet(CObj* pTarget)
{
    if (!pTarget)
        return;
    //미사일은 머리에서 발사됨

    for (int i = 0; i < 2; ++i)
    {
        CGBullet* pBullet = new CGBullet;
        pBullet->Initialize();
        //발사 위치 : 골리앗 중심 + 머리 방향으로의 오프셋
        float offsetDist = 1.f;  // 머리에서 약간 앞쪽에서 발사
        float startX = (m_tInfo.fX - i * 50) + m_vHeadDir.fX * offsetDist;
        float startY = (m_tInfo.fY - i * 50) + m_vHeadDir.fY * offsetDist;

        pBullet->Set_Pos(startX, startY);
        pBullet->Set_Target(pTarget);
        pBullet->Set_Owner(this);

        // 방향 설정 (타겟 방향)
        Vec2 vMyPos = { startX, startY };
        Vec2 vTargetPos = { pTarget->Get_Info().fX, pTarget->Get_Info().fY };
        Vec2 vDir = { vTargetPos.fX - vMyPos.fX, vTargetPos.fY - vMyPos.fY };

        float fLength = sqrtf(vDir.fX * vDir.fX + vDir.fY * vDir.fY);
        if (fLength > 0.f)
        {
            vDir.fX /= fLength;
            vDir.fY /= fLength;
        }

        pBullet->Set_Dir(vDir);

        // 투사체 방향은 머리 방향 사용
        pBullet->Set_Direction16(m_iHeadFrame);

        // ObjMgr에 추가
        CObjMgr::Get_Instance()->Add_Object(OBJID::OBJ_PROJECTILE, pBullet);
    }
}



