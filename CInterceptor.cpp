#include "pch.h"
#include "CInterceptor.h"
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
#include "CIntercepBullet.h"
#include "CCarrier.h"

CInterceptor::CInterceptor()
{
}

CInterceptor::~CInterceptor()
{
    if (m_pOwner && !m_pOwner->IsDead())
    {
        m_pOwner->InterceptorDestroyed(this);
    }
    Release();
}

void CInterceptor::Initialize()
{
    m_tInfo.fCX = 32.f; //메딕 한 칸 크기
    m_tInfo.fCY = 32.f;
    m_iMaxHP = 25;
    m_iHP = m_iMaxHP;
    m_iMaxShield = 25;
    m_iShield = m_iMaxShield;
    m_fSpeed = 200.f;
    //공격 관련 변수
    m_iAttackDamage = 5;
    m_fAttackRange = 10.f;
    m_fAttackSpeed = 1.0f;
    //공격 애니메이션 프레임 
    m_iAttackFrameStart = 7;
    m_iAttackFrameEnd = 12;

    m_fAttackInterval = 0.5f; //진짜 공격 속도!
    m_fAttackTimer = m_fAttackInterval;

    //다른 유닛이 공격 못하도록 설정
    m_bSelectable = false;

    //m_pFrameKey = L"Medic";

    m_eOriginalRace = eRaceType::RACE_PROTOSS;
    m_eCurrentRace = eRaceType::RACE_PROTOSS;

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::INTERCEPTOR;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 3;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;
}

int CInterceptor::Update()
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
        break;
    case eUnitState::ATTACK:
        if (m_bAttack)
        {
            //방향에 따른 행 설정
            m_tFrame.iFrame = DirTo17WayIndex(m_vDir);
            FireBullet();
            m_bAttack = false;
            m_eState = eUnitState::IDLE;
        }
        break;
    case eUnitState::DIE:
        //UpdateDead();
        return DEAD;
    default:
        break;
    }

    __super::Update_Rect();

    return NOEVENT;
}

void CInterceptor::Late_Update()
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

void CInterceptor::Render(HDC hDC)
{
    //전장의 안개 
    CUnit::Render(hDC);

    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - iScrollY);

    int fileIndex = 16;  // 기본값: 정면 (180도)
    bool bFlip = false;

    int dir16 = m_tFrame.iFrame;  // 0~15

    if (dir16 <= 8)  // 0~8: 오른쪽 반구 (0~180도)
    {
        // dir16 = 0 → 파일 0~1
        // dir16 = 1 → 파일 2~3
        // dir16 = 8 → 파일 16 (정면)
        if (dir16 == 8)
            fileIndex = 16;  // 정면
        else
            fileIndex = dir16 * 2 + (m_tFrame.iStart % 2);
    }
    else  // 9~15: 왼쪽 반구 (181~359도) - 좌우반전
    {
        // dir16 = 9 → dir16 = 7의 반전 → 파일 14~15
        // dir16 = 15 → dir16 = 1의 반전 → 파일 2~3
        int mirrorDir = 16 - dir16;  // 15→1, 14→2, ..., 9→7
        fileIndex = mirrorDir * 2 + (m_tFrame.iStart % 2);
        bFlip = true;
    }

    TCHAR szKey[256];
    switch (m_eState)
    {
    case eUnitState::IDLE:
    case eUnitState::MOVE:
        wsprintf(szKey, L"Interceptor_Move_%d", fileIndex);
        break;
    case eUnitState::ATTACK:
        wsprintf(szKey, L"Interceptor_Attack_%d", fileIndex);
        break;
    case eUnitState::DIE:
        break;
    default:
        wsprintf(szKey, L"Interceptor_Move_%d", fileIndex);
        break;
    }
    //wsprintf(szKey, L"Interceptor_Move_%d", fileIndex);

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

void CInterceptor::Release()
{
}

void CInterceptor::UpdateHotKeys()
{
    CUnit::UpdateHotKeys();
}

bool CInterceptor::ExecuteCommand(eCommandID command, CommandContext& context)
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

void CInterceptor::CommandCardSlot(vector<CommandSlot>& outSlot)
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

bool CInterceptor::UpdateMove(Order& order)
{
    // 현재 목표가 되는 웨이포인트
    Vec2 target = order.pTarget->Get_Pos();
    Vec2 current = { m_tInfo.fX, m_tInfo.fY };
    Vec2 diff = { target.fX - current.fX, target.fY - current.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
    // 도착 체크
    float fArriveEps = 20.f;

    if (dist <= fArriveEps) //Move 완료, 도착!
    {
        // 최종 도착 -> 정지
        Set_Dead();
        return true;
    }
    // 현재 위치와 vector<Vec2> path {}{}{} ipathindex의 위치를 통한 방향 * 속도만큼 이동!
    Vec2 dir = { diff.fX / dist, diff.fY / dist };
    // 방향 갱신
    m_vDir = dir;
    // 위치 업데이트
    float dt = CTimeMgr::Get_Instance()->GetDT();
    m_tInfo.fX += dir.fX * dt * m_fSpeed;
    m_tInfo.fY += dir.fY * dt * m_fSpeed;
    //목적지 도달 못했으므로 계속 이동!
    return false;
}

bool CInterceptor::UpdateRAttack(Order& order)
{
    //공격 중이 아닐 때만 타겟 설정
    if (!m_bAttack && order.pTarget)
    {
        m_pTarget = order.pTarget;
    }

    float fDT = CTimeMgr::Get_Instance()->GetDT();

    //타겟이 죽었거나 사라진 경우
    if (!order.pTarget || order.pTarget->IsDead())
    {
        //캐리어로 복귀
        Order retreat;
        retreat.eType = eOrderType::MOVE;
        retreat.pTarget = m_pOwner;
        //retreat.dst = Get_Pos();
        ClearOrder();
        PushOrder(retreat);
    }

    Vec2 targetPos = order.pTarget->Get_Pos();
    Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
    //타겟까지의 거리
    Vec2 diff = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);

    if (!m_bChasing) //후퇴
    {
        //Move -> Idle로 상태를 변환해서 공격 타이머가 돌 수 있도록 설정한다.
        if (m_eState == eUnitState::MOVE)
        {
            m_eState = eUnitState::IDLE;
        }
        float fMoveDist = m_fSpeed * fDT;
        m_tInfo.fX += m_vBackwardDir.fX * fMoveDist;
        m_tInfo.fY += m_vBackwardDir.fY * fMoveDist;
        m_fBackwardCur += fMoveDist;
        //후퇴 체크
        if (m_fBackwardCur >= m_fBackwardDist)
        {
            m_bChasing = true;
            m_fBackwardCur = 0.f;
        }
        return false;
    }
    else //타겟으로 돌진
    {
        //타겟 방향 보기
        if (dist > 0.1f)
        {
            m_vDir = { diff.fX / dist, diff.fY / dist };
        }

        m_tInfo.fX += m_vDir.fX * fDT * m_fSpeed;
        m_tInfo.fY += m_vDir.fY * fDT * m_fSpeed;

        if (dist <= m_fAttackRange)
        {
            //후퇴 방향 설정
            float angle = ((float)(rand() % 360)) * PI / 180.f;
            m_vBackwardDir = { cosf(angle), sinf(angle) };
            m_bChasing = false;
        }

        return false;
    }
}

void CInterceptor::UpdateDead()
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

void CInterceptor::FireBullet()
{
    if (!m_pTarget) //안전장치
        return;

    Vec2 targetPos = m_pTarget->Get_Pos();
    Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
    //타겟까지의 거리
    Vec2 diff = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
    //공격 사거리 체크
    if (dist <= m_fBackwardDist)
    {
        //타겟 방향 보기
        if (dist > 0.1f)
        {
            m_vDir = { diff.fX / dist, diff.fY / dist };
        }
        //히트스캔
        m_pTarget->TakeDamage(m_iAttackDamage);
        //사운드 재생
        //CSoundMgr::Get_Instance()->PlayEffect(L"intercep/pinlau00.wav", 0.5f);
        
        // 투사체 생성
        CIntercepBullet* pBullet = new CIntercepBullet;
        pBullet->Initialize();
        pBullet->Set_Pos(m_tInfo.fX, m_tInfo.fY);
        pBullet->Set_Target(m_pTarget);
        pBullet->Set_Owner(this);
        pBullet->Set_Homing(true); //타겟 따라가도록 설계
        pBullet->Set_Dir(m_vDir);
        pBullet->Set_Direction16(m_tFrame.iFrame);

        // ObjMgr에 추가
        CObjMgr::Get_Instance()->Add_Object(OBJID::OBJ_PROJECTILE, pBullet);
        return;
    }
}

