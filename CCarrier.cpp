#include "pch.h"
#include "CCarrier.h"
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
#include "CInterceptor.h"

CCarrier::CCarrier()
{
}

CCarrier::~CCarrier()
{
    Release();
}

void CCarrier::Initialize()
{
    m_tInfo.fCX = 128.f; //메딕 한 칸 크기
    m_tInfo.fCY = 128.f;
    m_iMaxHP = 500;
    m_iHP = m_iMaxHP;
    m_iMaxShield = 500;
    m_iShield = m_iMaxShield;
    m_fSpeed = 100.f;
    //공격 관련 변수
    m_iAttackDamage = 0;
    m_fAttackRange = 180.f;
    m_fAttackSpeed = 1.0f;
    //공격 애니메이션 프레임
    m_iAttackFrameStart = 7;
    m_iAttackFrameEnd = 12;

    m_eOriginalRace = eRaceType::RACE_PROTOSS;
    m_eCurrentRace = eRaceType::RACE_PROTOSS;

    m_eLayer = eUnitLayer::AIR; //공중 유닛 

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::CARRIER;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 3;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;
}

int CCarrier::Update()
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
        break;
    case eUnitState::MOVE:
        m_tFrame.iFrame = DirTo17WayIndex(m_vDir);
        break;
    case eUnitState::ATTACK:
        m_tFrame.iFrame = DirTo17WayIndex(m_vDir);
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

void CCarrier::Late_Update()
{
    //선택이 되었을 경우 마우스 방향의 애니메이션 재생
    if (!m_bSelected) return;
    //마우스 방향 안 보기!
    if (m_eState == eUnitState::MOVE) return;
    if (m_eState == eUnitState::ATTACK) return;
    if (m_eState == eUnitState::IDLE) return;

    Vec2 vWorldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    Vec2 vDir{ vWorldMouse.fX - m_tInfo.fX, vWorldMouse.fY - m_tInfo.fY };

    int iDir17 = DirTo17WayIndex(vDir);
    m_tFrame.iFrame = iDir17;
}

void CCarrier::Render(HDC hDC)
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
    wsprintf(szKey, L"Carrier_Move_%d", fileIndex);

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

void CCarrier::Release()
{
}

void CCarrier::UpdateHotKeys()
{
    CUnit::UpdateHotKeys();
}

bool CCarrier::ExecuteCommand(eCommandID command, CommandContext& context)
{
    //먼저 부모 명령 실행
    if (CUnit::ExecuteCommand(command, context))
        return true;

    // CCommandMgr로 위임, 부모 명령 실행 이후 진행
    switch (command)
    {
    case eCommandID::INTERCEPTOR:
        RetreatInterceptor();
        break;
    }
    return false;
}

void CCarrier::CommandCardSlot(vector<CommandSlot>& outSlot)
{
    CUnit::CommandCardSlot(outSlot);
    //7번 : INTERCEPT 사용
    outSlot[6].commandID = eCommandID::INTERCEPTOR;
    outSlot[6].iconKey = TEXT("ICON_STEAMPACK");
    outSlot[6].hotkey = 'X';
    outSlot[6].clickable = true;
    outSlot[6].visible = true;
    outSlot[6].lock = false;
}

void CCarrier::UpdateDead()
{
    if (m_eTeamType == eTeamType::ALLY)
    {
        CResourceMgr::Get_Instance()->SubtractSupply(2);
    }
    //이펙트와 사운드 재생
    CSoundMgr::Get_Instance()->PlayEffect(L"BattleCrusor/BattleCrusorDeath.wav", 1.f);

    //타겟 이펙트 생성
    CObj* pEffect = CAbstractFactory<CEffect>::Create(
        m_tInfo.fX, m_tInfo.fY);
    pEffect->Initialize();
    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    if (pEffectObj)
    {
        pEffectObj->Set_Effect(L"DarkTemplar_Die_",
            7, 128, 128, eEffectType::COL_BASE, RGB(0, 0, 0), 0.05f, false,
            eEffectImageType::PNG);
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
}

bool CCarrier::UpdateAttackMove(Order& order)
{
    if (m_bAttackMove)
    {
        m_pTarget = FindNearestEnemy(m_fAttackRange);
        if (m_pTarget)
        {
            order.eType = eOrderType::ATTACK;
            order.pTarget = m_pTarget;
            return false; //dst, path, ipathindex에 대한 정보는 유지가 핵심
        }
    }

    if (order.path.empty())
    {
        m_velocity.fX = 0.f;
        m_velocity.fY = 0.f;
        m_eState = eUnitState::IDLE;
        return false;  // A땅은 대기
    }
    if (order.iPathIndex >= (int)order.path.size())
    {
        m_velocity.fX = 0.f;
        m_velocity.fY = 0.f;
        m_eState = eUnitState::IDLE;
        return false;  // A땅은 도착해도 계속
    }

    Vec2 target = order.path[order.iPathIndex];
    Vec2 current = { m_tInfo.fX, m_tInfo.fY };
    Vec2 diff = { target.fX - current.fX, target.fY - current.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);

    if (dist <= 6.f)
    {
        order.iPathIndex++;
        if (order.iPathIndex >= (int)order.path.size())
        {
            m_velocity.fX = 0.f;
            m_velocity.fY = 0.f;
            m_eState = eUnitState::IDLE;
            return false;  // 도착
        }
        return false;
    }

    // 이동!
    Vec2 dir = { diff.fX / dist, diff.fY / dist };
    m_vDir = dir;
    m_velocity.fX = dir.fX * m_fSpeed;
    m_velocity.fY = dir.fY * m_fSpeed;

    float dt = CTimeMgr::Get_Instance()->GetDT();
    m_tInfo.fX += dir.fX * dt * m_fSpeed;
    m_tInfo.fY += dir.fY * dt * m_fSpeed;

    return false;
}

bool CCarrier::UpdateRAttack(Order& order)
{
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

    m_pTarget = order.pTarget;

    float fDT = CTimeMgr::Get_Instance()->GetDT();

    Vec2 targetPos = order.pTarget->Get_Pos();
    Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
    //타겟까지의 거리
    Vec2 diff = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
    //공격 사거리 체크
    if (dist <= m_fAttackRange)
    {
        if (m_bSpawnIntercep)
            return true;
        FireBullet();
        m_bSpawnIntercep = true;
        //타겟 방향 보기
        if (dist > 0.1f)
        {
            m_vDir = { diff.fX / dist, diff.fY / dist };
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

void CCarrier::InterceptorDestroyed(CInterceptor* pIntercep)
{
    //인터셉터 벡터 비우기
    auto it = find(m_vecInterceptor.begin(), m_vecInterceptor.end(), pIntercep);
    if (it != m_vecInterceptor.end())
    {
        m_vecInterceptor.erase(it);
    }
}

void CCarrier::RetreatInterceptor()
{
    if (m_vecInterceptor.empty())
        return;
    //소환한 인터셉터 회수하기
    for (auto& pInterceptor : m_vecInterceptor)
    {
        Order retreat;
        retreat.eType = eOrderType::MOVE;
        retreat.pTarget = this;
        //retreat.dst = Get_Pos();
        pInterceptor->ClearOrder();
        pInterceptor->PushOrder(retreat);
    }
    //벡터 비우기
    m_vecInterceptor.clear();
    //다시 인터셉터 소환할 수 있게 플래그 설정
    m_bSpawnIntercep = false;
}

void CCarrier::FireBullet()
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
        m_pTarget->TakeDamage(m_iAttackDamage);
        //사운드 재생
        CSoundMgr::Get_Instance()->PlayEffect(L"Medic/MedicHeal.wav", 5.f);
        //적 위치로 interceptor 6기 소환하기
        Vec2 pos = Get_Pos();
        for (int i = 0; i < 6; ++i)
        {
            CObj* pInterceptor = CAbstractFactory<CInterceptor>::Create(pos.fX, pos.fY);
            CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pInterceptor);

            //타겟 지점으로 이동
            CInterceptor* ppInterceptor = dynamic_cast<CInterceptor*>(pInterceptor);
            ppInterceptor->SetOwner(this);
            //vector에 pushback하기
            m_vecInterceptor.push_back(ppInterceptor);
            if (ppInterceptor)
            {
                Order targetOrder;
                targetOrder.eType = eOrderType::ATTACK;
                targetOrder.pTarget = m_pTarget;
                targetOrder.dst = targetPos;
                ppInterceptor->PushOrder(targetOrder);
            }
        }
        return;
    }
}
