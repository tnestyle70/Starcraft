#include "pch.h"
#include "CMarine.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"

CMarine::CMarine()
{
}

CMarine::~CMarine()
{
    Release();
}

void CMarine::Initialize()
{
    m_tInfo.fCX = 50.f; //마린 한 칸 크기
    m_tInfo.fCY = 50.f;
    m_iMaxHP = 100; 
    m_iHP = m_iMaxHP;
    m_fSpeed = 200.f;
    //공격 관련 변수
    m_iAttackDamage = 6;
    m_fAttackRange = 120.f;
    m_fAttackSpeed = 1.0f;
    //공격 애니메이션 프레임 
    iAttackFrameStart = 11;
    iAttackFrameEnd = 14;

    m_pFrameKey = L"Marine";

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::MARINE;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 7;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0; 
    m_tFrame.dwSpeed = 100; 
}

int CMarine::Update()
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
            if (m_tFrame.iStart < iAttackFrameStart ||
                m_tFrame.iStart > iAttackFrameEnd)
            {
                m_tFrame.iStart = iAttackFrameStart;
            }
            else
            {
                m_tFrame.iStart++;
                if (m_tFrame.iStart > iAttackFrameEnd)
                {
                    m_tFrame.iStart = iAttackFrameStart;  // 반복
                }
            }
            m_tFrame.dwTime = now;
        }
        break;
    case eUnitState::DIE:
        break;
    default:
        break;
    }

    __super::Update_Rect();

    return NOEVENT;
}

void CMarine::Late_Update()
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

void CMarine::Render(HDC hDC)
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
}

void CMarine::Release()
{
}
