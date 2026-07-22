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
#include "CSoundMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"
#include "CGMissile.h"

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
    m_fSpeed = 100.f;
    //공격 변수 초기화
    m_iAttackDamage = 10;
    m_fAttackRange = 140.f;

    m_fAttackInterval = 0.7f; //진짜 공격 속도!
    m_fAttackTimer = m_fAttackInterval;

    m_pFrameKey = L"Goliath_Body";
    m_pHeadKey = L"Goliath_Head";

    m_eOriginalRace = eRaceType::RACE_TERRAN;
    m_eCurrentRace = eRaceType::RACE_TERRAN;

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::GOLIATH;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 9;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;

    //포탑 초기화
    m_vHeadDir = { 0.f, -1.f }; //위쪽 방향
    m_iHeadFrame = 0;
    m_iFireFrame = 10;
}

int CGoliath::Update()
{
    int iResult = CUnit::Update();

    if (iResult == DEAD)
        m_eState = eUnitState::DIE;

    //적일 경우 AI 업데이트, 아니면 조작하면서 게임 플레이 가능하도록 만들기
    if (m_eTeamType == eTeamType::ENEMY)
    {
        //AI업데이트
        UpdateAI();
    }

    UpdateBody();
    if (m_eState == eUnitState::DIE)
        return DEAD;
    UpdateHead();

    __super::Update_Rect();

    return NOEVENT;
}

void CGoliath::Late_Update()
{
    //선택이 되었을 경우 마우스 방향의 애니메이션 재생
    if (!m_bSelected) return;
    //마우스 방향 안 보기!
    if (m_eState == eUnitState::MOVE) return;
    if (m_eState == eUnitState::ATTACK) return;
    if (m_eState == eUnitState::IDLE) return;

    Vec2 vWorldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    Vec2 vDir{ vWorldMouse.fX - m_tInfo.fX, vWorldMouse.fY - m_tInfo.fY };

    int iDir16 = DirTo16WayIndex(vDir);
    m_tFrame.iFrame = iDir16;
    m_iHeadFrame = iDir16;
}

void CGoliath::UpdateBody()
{
    DWORD now = GetTickCount();

    //몸체는 이동 방향 따라가기
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
        //공격 중에도 몸체는 이동 방향 유지
        break;
    case eUnitState::DIE:
        UpdateDead();
        m_eState = eUnitState::DIE;
        return;
    default:
        break;
    }
}

void CGoliath::UpdateHead()
{
    DWORD now = GetTickCount();
    float dt = CTimeMgr::Get_Instance()->GetDT();

    switch (m_eState)
    {
    case eUnitState::IDLE:
        m_iFireFrame = 0; //항상 발사 프레임 0으로 초기화
        m_iHeadFrame = 0;
        if (m_pTarget)
        {
            //타겟이 존재하면 머리 타겟 방향 바라보도록 설정 
            m_iHeadFrame = DirTo16WayIndex(m_vHeadDir);

            m_fAttackTimer += dt;
            if (m_fAttackTimer >= m_fAttackInterval)
            {
                m_eState = eUnitState::ATTACK;
                m_fAttackTimer = 0.f;
                m_bAttack = true;
                m_dwFireStartTime = now; //공격 상태로 이동한 시간 체크
            }
        }
        break;
    case eUnitState::MOVE:
        m_iFireFrame = 0; //항상 발사 프레임 0으로 초기화
        m_vHeadDir = m_vDir;
        m_iHeadFrame = DirTo16WayIndex(m_vHeadDir);
        //MOVE 상태에서도 발사 프레임 초기화!!
        //m_bFiring = false;
        break;
    case eUnitState::ATTACK:
        if (m_bAttack)
        {
            m_iFireFrame = 10;
            //머리는 타겟 방향 바라보도록 설정 
            m_iHeadFrame = DirTo16WayIndex(m_vHeadDir);
            //애니메이션 카운트 진행
            if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
            {
                m_iFireFrameTimer++;
                if (m_iFireFrameTimer == 1)
                {
                    JudgeLayer(); //공중, 지상 판단
                    //FireBullet();
                }
                if (m_iFireFrameTimer >= m_iFireFrameDuration)
                {
                    m_iFireFrame = 0;
                    m_bAttack = false;
                    m_eState = eUnitState::IDLE;
                    m_iFireFrameTimer = 0;
                }
                m_tFrame.dwTime = now;
            }
        }
        break;
    case eUnitState::DIE:
        return;
    default:
        break;
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

    //발사 중이면 발사 프레임으로 설정
    int iSrcX = m_bAttack ? (m_iFireFrame * (int)m_tInfo.fCX) : 0; 
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

bool CGoliath::UpdateRAttack(Order& order)
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
        if (m_bAttackMove)
        {
            order.eType = eOrderType::ATTACK_MOVE;
            order.pTarget = nullptr;
            return false;
        }
        else
        {
            m_eState = eUnitState::IDLE;
            return false; //오더 완료
        }
    }

    Vec2 targetPos = order.pTarget->Get_Pos();
    Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
    //타겟까지의 거리
    Vec2 diff = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
    //공격 사거리 체크
    if (dist <= m_fAttackRange)
    {
        //Move -> Idle로 상태를 변환해서 공격 타이머가 돌 수 있도록 설정한다.
        if (m_eState == eUnitState::MOVE)
        {
            m_eState = eUnitState::IDLE;
        }
        //타겟 방향 보기
        if (dist > 0.1f)
        {
            //탱크는 머리 방향도 업데이트!
            m_vDir = { diff.fX / dist, diff.fY / dist };
            m_vHeadDir = { diff.fX / dist, diff.fY / dist };
        }
        return false;
    }
    else
    {
        //타겟이 사거리 내에 존재하지 않을 경우 이동
        Vec2 dir = { diff.fX / dist, diff.fY / dist };
        m_vDir = dir;
        float fDT = CTimeMgr::Get_Instance()->GetDT();
        m_tInfo.fX += dir.fX * fDT * m_fSpeed;
        m_tInfo.fY += dir.fY * fDT * m_fSpeed;
        return false;
    }
}

void CGoliath::UpdateDead()
{
    if (m_eTeamType == eTeamType::ALLY)
    {
        CResourceMgr::Get_Instance()->SubtractSupply(2);
    }
    //이펙트와 사운드 재생
    CSoundMgr::Get_Instance()->PlayEffect(L"Goliath/TGoDth00.wav", 1.f);

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

void CGoliath::JudgeLayer()
{
    if (!m_pTarget)
        return;
    eUnitLayer layer = m_pTarget->GetLayer();
    switch (layer)
    {
    case eUnitLayer::GROUND:
        FireBullet();
        break;
    case eUnitLayer::AIR:
        FireMissile();
        break;
    default:
        break;
    }
}

void CGoliath::FireBullet()
{
    if (!m_pTarget) //안전장치
        return;

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
        //히트스캔
        m_pTarget->TakeDamage(m_iAttackDamage);
        //사운드 재생
        CSoundMgr::Get_Instance()->PlayEffect(L"Goliath/TGoFir00.wav", 0.6f);
        //이펙트 생성
        m_fAttackSpeed = 2.5f;
        return;
    }
}

void CGoliath::FireMissile()
{
    if (!m_pTarget) //안전장치
        return;

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
        //공격 사운드 재생
        CSoundMgr::Get_Instance()->PlayEffect(L"Goliath/hkmissle.wav", 0.5f);

        //현재 방향 가져오기 (16방향)
        int iCurrentDir = m_tFrame.iFrame;

        // 투사체 생성
        CGMissile* pBullet = new CGMissile;
        pBullet->Initialize();
        pBullet->Set_Pos(m_tInfo.fX, m_tInfo.fY);
        pBullet->Set_Target(m_pTarget);
        pBullet->Set_Owner(this);
        pBullet->Set_Homing(true); //타겟 따라가도록 설계
        pBullet->Set_Dir(m_vHeadDir);

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
}

void CGoliath::UpdateAI()
{
    if (m_eTeamType == eTeamType::ALLY)
        return;
    //주변 적 찾기
    CObj* pEnemy = FindNearestEnemyAI(m_fAttackRange);
    //주변에 적이 존재할 경우, 오더 다 멈추고 공격 Order 추가
    if (pEnemy)
    {
        if (!m_OrderQ.empty())
        {
            m_OrderQ.pop_front();
        }
        Order attack;
        attack.eType = eOrderType::ATTACK;
        attack.pTarget = pEnemy;
        attack.dst = { pEnemy->Get_Info().fX, pEnemy->Get_Info().fY };

        m_OrderQ.push_back(attack);
    }
    else
    {
        //Order stop;
        //stop.eType = eOrderType::STOP;
        //m_OrderQ.push_back(stop);
        //Order move;
        //move.eType = eOrderType::MOVE;
        //move.dst = { 0,0 };  // ← 문제 2: 매 프레임 (0,0) 명령 추가!
        //m_OrderQ.push_back(move);  // ← 큐가 무한정 쌓임!
    }
}

CObj* CGoliath::FindNearestEnemyAI(float searchRadius)
{
    float fMinDistance = FLT_MAX;
    CObj* pNearestEnemy = nullptr;

    // 탐지 범위 (사거리 + 추가 탐지 범위)
    float fDetectionRange = m_fAttackRange + 300.f; //300 감지 범위

    // 모든 유닛 검사
    auto& unitList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_UNIT);
    for (auto& pObj : unitList)
    {
        // 자기 자신 제외
        if (pObj == this)
            continue;

        // 죽은 유닛 제외
        if (pObj->Is_Dead())
            continue;

        //벙커에 있는 유닛 제외
        if (!pObj->IsSelectable())
            continue;

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

