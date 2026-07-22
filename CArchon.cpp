#include "pch.h"
#include "CArchon.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CSoundMgr.h"
#include "CTimeMgr.h"
#include "CEffect.h"
#include "CObjMgr.h"
#include "CAbstractFactory.h"
#include "CUIMgr.h"
#include "CSelectionMgr.h"

CArchon::CArchon()
{
}

CArchon::~CArchon()
{
    Release();
}

void CArchon::Initialize()
{
    m_tInfo.fCX = 64.f; //메딕 한 칸 크기
    m_tInfo.fCY = 64.f;
    m_iMaxHP = 150;
    m_iHP = m_iMaxHP;
    m_iMaxMP = 100;
    m_iShield = m_iMaxShield;
    m_fSpeed = 100.f;
    //공격 관련 변수
    m_iAttackDamage = 6;
    m_fAttackRange = 40.f;
    m_fAttackSpeed = 1.0f;
    //공격 애니메이션 프레임 
    m_iAttackFrameStart = 7;
    m_iAttackFrameEnd = 12;

    m_fAttackInterval = 0.1f; //진짜 공격 속도!
    m_fAttackTimer = m_fAttackInterval;

    //m_pFrameKey = L"Medic";

    m_eOriginalRace = eRaceType::RACE_PROTOSS;
    m_eCurrentRace = eRaceType::RACE_PROTOSS;

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::ARCHON;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 3;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;
}

int CArchon::Update()
{
    int iResult = CUnit::Update();

    if (iResult == DEAD)
        m_eState = eUnitState::DIE;

    DWORD now = GetTickCount();
    float dt = CTimeMgr::Get_Instance()->GetDT();

    switch (m_eState)
    {
    case eUnitState::IDLE:
        m_tFrame.iStart = 0;
        if (m_pTarget)
        {
            m_fAttackTimer += dt;
            if (m_fAttackTimer >= m_fAttackInterval)
            {
                m_eState = eUnitState::ATTACK;
                m_fAttackTimer = 0.f;
                m_bAttack = true;
            }
        }
        break;
    case eUnitState::MOVE:
        m_tFrame.iFrame = DirTo17WayIndex(m_vDir);

        if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
        {
            m_tFrame.iStart++;
            if (m_tFrame.iStart > m_tFrame.iEnd)
                m_tFrame.iStart = 0;
            m_tFrame.dwTime = now;
        }
        break;
    case eUnitState::ATTACK:
        if (m_bAttack)
        {   //방향에 따른 행 설정
            m_tFrame.iFrame = DirTo17WayIndex(m_vDir);
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
                        FireBullet();
                        m_bAttack = false;
                        m_eState = eUnitState::IDLE;
                    }
                }
                m_tFrame.dwTime = now;
            }
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

void CArchon::Late_Update()
{
    //선택이 되었을 경우 마우스 방향의 애니메이션 재생
    if (!m_bSelected) return;
    //마우스 방향 안 보기!
    //if (m_eState == eUnitState::MOVE) return;
    //if (m_eState == eUnitState::ATTACK) return;
    if (m_eState != eUnitState::IDLE) return;

    Vec2 vWorldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    Vec2 vDir{ vWorldMouse.fX - m_tInfo.fX, vWorldMouse.fY - m_tInfo.fY };

    int iDir17 = DirTo17WayIndex(vDir);
    m_tFrame.iFrame = iDir17;
}

void CArchon::Render(HDC hDC)
{
    if (!m_bVisible) //벙커에 탑승해있는 상태일 경우 렌더링X
        return;

    // 전장의 안개
    CUnit::Render(hDC);

    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - iScrollY);

    //몸체 렌더링
    int bodyFrame = (GetTickCount() / 100) % 10;  // 0~9 순환

    TCHAR szBodyKey[256];
    wsprintf(szBodyKey, L"Archon_Body_%d", bodyFrame);

    CMyPng* pOrbPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szBodyKey);

    if (pOrbPng)
    {
        int iWidth = pOrbPng->Get_Width();
        int iHeight = pOrbPng->Get_Height();
        int drawX = (int)m_tInfo.fX - iScrollX - iWidth / 2;
        int drawY = (int)m_tInfo.fY - iScrollY - iHeight / 2;

        pOrbPng->Render_Alpha(hDC, drawX, drawY, 
            iWidth, iHeight, false, m_bConceal);
    }

    //실체 렌더링
    int dir16 = m_tFrame.iFrame;        // 방향 0~16
    bool bFlip = false;

    // 좌우 반전 처리
    if (dir16 <= 8)  // 9~16은 0~8의 반전
    {
        if (dir16 == 8)
            dir16 = 16; //정면
        else
            dir16 = dir16 * 2 + (m_tFrame.iStart % 2);
    }
    else
    {
        int mirrorDir = 16 - dir16;  // 15→1, 14→2, ..., 9→7
        dir16 = mirrorDir * 2 + (m_tFrame.iStart % 2);
        bFlip = true;
    }

    // 스프라이트 키 생성 (Import 키 구조와 동일)
    TCHAR szKey[256];
    wsprintf(szKey, L"Archon_Move%d_Anim%d", dir16, m_tFrame.iStart);

    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szKey);
    if (pPng)
    {
        int iWidth = pPng->Get_Width();
        int iHeight = pPng->Get_Height();

        if (bFlip)
            pPng->Render_Alpha_Flipped(hDC,
                (int)m_tInfo.fX - iScrollX - iWidth / 2,
                (int)m_tInfo.fY - iScrollY - iHeight / 2, 
                iWidth, iHeight, false, m_bConceal);
        else
            pPng->Render_Alpha(hDC,
                (int)m_tInfo.fX - iScrollX - iWidth / 2,
                (int)m_tInfo.fY - iScrollY - iHeight / 2,
                iWidth, iHeight, false, m_bConceal);
    }
}

void CArchon::Release()
{
}

void CArchon::UpdateHotKeys()
{
    CUnit::UpdateHotKeys();
}

bool CArchon::ExecuteCommand(eCommandID command, CommandContext& context)
{
    //먼저 부모 명령 실행
    if (CUnit::ExecuteCommand(command, context))
        return true;

    // CCommandMgr로 위임, 부모 명령 실행 이후 진행
    switch (command)
    {
    case eCommandID::RESTORATION:
        break;
    case eCommandID::OPTICAL_FLARE:
        break;
    case eCommandID::CADUCEUS_REACTOR:
        break;
    }
    return false;
}

void CArchon::CommandCardSlot(vector<CommandSlot>& outSlot)
{
    CUnit::CommandCardSlot(outSlot);
    //7번 : RESTORATION 사용
    outSlot[6].commandID = eCommandID::RESTORATION;
    outSlot[6].iconKey = TEXT("ICON_STEAMPACK");
    outSlot[6].hotkey = 'X';
    outSlot[6].clickable = true;
    outSlot[6].visible = true;
    outSlot[6].lock = true;
    //8번 : OPTICAL_FLARE 사용
    outSlot[7].commandID = eCommandID::OPTICAL_FLARE;
    outSlot[7].iconKey = TEXT("ICON_STEAMPACK");
    outSlot[7].hotkey = 'C';
    outSlot[7].clickable = true;
    outSlot[7].visible = true;
    outSlot[7].lock = true;
    //9번 : CADUCEUS_REACTOR 사용
    outSlot[8].commandID = eCommandID::CADUCEUS_REACTOR;
    outSlot[8].iconKey = TEXT("ICON_STEAMPACK");
    outSlot[8].hotkey = 'V';
    outSlot[8].clickable = true;
    outSlot[8].visible = true;
    outSlot[8].lock = true;
}

void CArchon::UpdateDead()
{
    //이펙트와 사운드 재생
    CSoundMgr::Get_Instance()->PlayEffect(L"BattleCrusor/BattleCrusorDeath.wav", 1.f);

    //타겟 이펙트 생성
    CObj* pEffect = CAbstractFactory<CEffect>::Create(
        m_tInfo.fX, m_tInfo.fY);
    pEffect->Initialize();
    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    if (pEffectObj)
    {
        pEffectObj->Set_Effect(L"MARINE_DEATH_EFFECT",
            7, 100, 50, eEffectType::COL_BASE, RGB(255, 255, 0));
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
}

void CArchon::FireBullet()
{
    if (!m_pTarget) //안전장치
        return;

    Vec2 targetPos = m_pTarget->Get_Pos();
    Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
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
        //히트스캔
        m_pTarget->Heal(m_iAttackDamage);
        //사운드 재생
        CSoundMgr::Get_Instance()->PlayEffect(L"Medic/MedicHeal.wav", 5.f);
        return;
    }
}
