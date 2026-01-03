#include "pch.h"
#include "CTank.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CUIMgr.h"
#include "CSelectionMgr.h"
#include "CTimeMgr.h"
    
CTank::CTank()
{
}

CTank::~CTank()
{
    Release();
}

void CTank::Initialize()
{
    m_tInfo.fCX = 128.f; //마린 한 칸 크기
    m_tInfo.fCY = 128.f;
    m_iMaxHP = 300;
    m_iHP = m_iMaxHP;
    m_fSpeed = 200.f;
    //공격 변수 초기화
    m_iAttackDamage = 20;
    m_fAttackRange = 7.f * TILECX;
    m_fAttackSpeed = 1.f;

    m_pFrameKey = L"Tank_Body";
    m_pHeadKey = L"Tank_Head";

    m_eRender = RENDER_WORLD;
    m_eCommandCardState = eCommandCardState::NORMAL_TANK;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::TANK;
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
    //모드 초기화 
    m_bTransforming = false;
    m_bSiegeMode = false;
    m_fTankRange = 7.f * TILECX;
    m_fSiegeRange = 12.f * TILECX;
}

int CTank::Update()
{
    int iResult = CUnit::Update();

    if (iResult == DEAD)
        return DEAD;

    UpdateBody();
    UpdateHead();

    __super::Update_Rect();

    return NOEVENT;
}

void CTank::Late_Update()
{
    //선택이 되었을 경우 마우스 방향의 애니메이션 재생
    if (!m_bSelected) return;
    //이동 중이면 마우스 방향 애니메이션 재생 멈추기
    if (m_eState != eUnitState::IDLE) return;

    Vec2 vWorldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    Vec2 vDir{ vWorldMouse.fX - m_tInfo.fX, vWorldMouse.fY - m_tInfo.fY };

    int iDir16 = DirTo16WayIndex(vDir);
    if (!m_bSiegeMode && !m_bTransforming)
    {
        m_tFrame.iFrame = iDir16;
    }
}

void CTank::ToggleSiegeMode()
{
    //변신하고 있을 때는 입력 X
    if (m_bTransforming) return;

    m_bTransforming = true;
    m_fSpeed = 0.f;
    m_tFrame.iStart = 3; //col 3 변신 애니메이션

    if (m_bSiegeMode)
    {
        //시즈 -> 일반 역재생(5 -> 0)
        m_tFrame.iFrame = 5;
        m_tFrame.iEnd = 0;
    }
    else
    {
        //일반 -> 시즈 정방향
        m_tFrame.iFrame = 0;
        m_tFrame.iEnd = 5;
    }

    m_tFrame.dwTime = GetTickCount();
    m_tFrame.dwSpeed = 200;
}

void CTank::UpdateBody()
{
    //변신 중일 경우
    if (m_bTransforming)
    {
        DWORD now = GetTickCount();
        if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
        {
            //시즈 해제 중이면 감소, 시즈 모드 중이면 증가
            if (m_bSiegeMode)
                m_tFrame.iFrame--;
            else
                m_tFrame.iFrame++;

            m_tFrame.dwTime = now;
            //애니메이션 끝 체크
            bool bFinished = m_bSiegeMode ?
                (m_tFrame.iFrame <= 0) : (m_tFrame.iFrame >= 5);

            if (bFinished)
            {
                m_bTransforming = false;
                m_bSiegeMode = !m_bSiegeMode;
                //시즈 모드인 경우 프레임 고정
                if (m_bSiegeMode)
                {
                    m_tFrame.iStart = 3;
                    m_tFrame.iFrame = 5;
                    m_fSpeed = 0.f;
                    m_fAttackRange = m_fSiegeRange;
                    m_eCommandCardState = eCommandCardState::SIEGE_TANK;
                }
                else
                {
                    m_tFrame.iStart = 0;
                    m_tFrame.iFrame = 0;
                    m_fSpeed = 200.f;
                    m_fAttackRange = m_fTankRange;
                    m_eCommandCardState = eCommandCardState::NORMAL_TANK;
                }
            }
        }
        return;
    }

    if (m_bSiegeMode)
    {
        m_tFrame.iStart = 3; //시즈모드일 경우 col 3 유지
        m_tFrame.iFrame = 5; //프레임 고정
        return;
    }
    //일반 탱크 모드
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

void CTank::UpdateHead()
{
    if (m_bSiegeMode) //시즈 모드일 경우 HeadFrame 2로 변경
        m_iHeadFrame = 2;

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
    DWORD now = GetTickCount();
    if (m_eState == eUnitState::ATTACK) 
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
            // col 2로 이동 (발사 프레임)
            m_iFireFrame = 1;
        }
        else
        {
            m_bFiring = false;
            m_iFireFrame = 0;  // col 0 = 일반 상태
        }
    }
}

void CTank::RenderBody(HDC hDC)
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

void CTank::RenderHead(HDC hDC)
{

    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    HDC hHeadDC = CBmpMgr::Get_Instance()->Find_Image(m_pHeadKey);

    // 포탑 이미지 구조에 따라 다름
    // 방법 1: 방향별 프레임이 세로로 배열

    int iSrcX = 0; //일반 모드 col 0
    if (m_bSiegeMode) //시즈 모드 col 2
        iSrcX = 2 * (int)m_tInfo.fCX;
    else if (m_bFiring) //공격 모드 col 1
        iSrcX = 1 * (int)m_tInfo.fCY;

    //int iScrX = m_bFiring ? (m_iFireFrame * (int)m_tInfo.fCX) : 0;  // 발사중이면 애니메이션
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

void CTank::Render(HDC hDC)
{
    //전장의 안개 
    CUnit::Render(hDC);

    RenderBody(hDC);

    if (!m_bTransforming)
    {
        RenderHead(hDC);
    }
}

void CTank::Release()
{
}

void CTank::UpdateHotKeys()
{
    CUnit::UpdateHotKeys();
    //SCV 유닛 하나만 선택되었을 경우 실행
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
    if (selected.size() != 1)
        return;
    //선택된 객체가 this인지 확인
    if (selected[0] != this)
        return;
    //슬롯 정보
    vector<CommandSlot> slots;
    this->CommandCardSlot(slots);
    //각 슬롯의 단축키 확인
    for (int i = 0; i < slots.size(); ++i)
    {
        if (!slots[i].visible || !slots[i].clickable)
            continue;
        //단축키가 눌렸는지 확인
        if (CInputMgr::Get_Instance()->KeyDownVK(slots[i].hotkey))
        {
            CUIMgr::Get_Instance()->SetButtonFeedback(i, true);
            //명령 실행
            CommandContext context{};
            this->ExecuteCommand(slots[i].commandID, context);
        }
    }
}

bool CTank::ExecuteCommand(eCommandID command, CommandContext& context)
{
    //먼저 부모 명령 실행
    if (CUnit::ExecuteCommand(command, context))
        return true;

    // CCommandMgr로 위임, 부모 명령 실행 이후 진행
    switch (command)
    {
    case eCommandID::SIEGE_TANK:
        ToggleSiegeMode(); //시즈 모드로 전환
        break;
    case eCommandID::TANK:
        ToggleSiegeMode(); //일반 모드로 전환
        break;
    }

    return false;
}

void CTank::CommandCardSlot(vector<CommandSlot>& outSlot)
{

    switch (m_eCommandCardState)
    {
    case eCommandCardState::NORMAL_TANK:
        CUnit::CommandCardSlot(outSlot);
        //7번 : 시즈 모드로 변경
        outSlot[6].commandID = eCommandID::SIEGE_TANK;
        outSlot[6].iconKey = TEXT("ICON_SIEGE_TANK");
        outSlot[6].hotkey = 'B';
        outSlot[6].clickable = true;
        outSlot[6].visible = true;
        break;
    case eCommandCardState::SIEGE_TANK:
        CUnit::CommandCardSlot(outSlot);
        //7번 : 탱크 모드로 변경
        outSlot[6].commandID = eCommandID::TANK;
        outSlot[6].iconKey = TEXT("ICON_TANK");
        outSlot[6].hotkey = 'B';
        outSlot[6].clickable = true;
        outSlot[6].visible = true;
        break;
    default:
        break;
    }
}

bool CTank::UpdateAttack(Order& order)
{
    //변신 중에는 공격 불가
    if (m_bTransforming)
    {
        m_eState = eUnitState::IDLE;
        return false;
    }
    // 타겟이 죽었거나 사라진 경우
    if (!order.pTarget || order.pTarget->IsDead())
    {
        m_eState = eUnitState::IDLE;
        return true; // 오더 완료
    }

    Vec2 targetPos = order.pTarget->Get_Pos();
    Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
    // 타겟까지의 거리
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
        }
        // 공격 쿨타임 체크
        DWORD now = GetTickCount();
        DWORD attackCoolTime = (DWORD)(1000.f / m_fAttackSpeed);
        if (now - m_dwLastAttack >= attackCoolTime)
        {
            order.pTarget->TakeDamage(m_iAttackDamage);
            m_dwLastAttack = now;
        }
        return false;
    }
    else
    {
        // 시즈 모드일 때는 이동 불가 - 타겟이 사거리 밖이면 공격 중단
        if (m_bSiegeMode)
        {
            m_eState = eUnitState::IDLE;
            return true; // 오더 완료 (타겟 포기)
        }

        // 일반 모드일 때는 타겟 추적
        m_eState = eUnitState::MOVE;
        Vec2 dir = { diff.fX / dist, diff.fY / dist };
        m_vDir = dir;
        float fDT = CTimeMgr::Get_Instance()->GetDT();
        m_tInfo.fX += dir.fX * fDT * m_fSpeed;
        m_tInfo.fY += dir.fY * fDT * m_fSpeed;
        return false;
    }
}



