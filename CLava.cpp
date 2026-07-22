#include "pch.h"
#include "CLava.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "Define.h"
#include <cmath>
#include "CInputMgr.h"
#include "CTileMgr.h"
#include "CFogMgr.h"
#include "CSoundMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"
#include "CSoundMgr.h"
#include "CTimeMgr.h"
#include "CCommandMgr.h"
#include "CSelectionMgr.h"
#include "CUIMgr.h"
#include "CResourceMgr.h"
//유닛
#include "CDrone.h"
#include "CZergling.h"
#include "COverload.h"
#include "CHydralisk.h"
#include "CMutalisk.h"
#include "CUltralisk.h"

CLava::CLava() 
{
}

CLava::~CLava()
{
    Release();
}

void CLava::Initialize()
{
    // Lava 크기 및 기본 정보
    m_tInfo.fCX = 36.f;
    m_tInfo.fCY = 36.f;

    // Drone 스탯
    m_iMaxHP = 100;
    m_iHP = m_iMaxHP;
    m_fSpeed = 50.f;

    m_eOriginalRace = eRaceType::RACE_ZERG;
    m_eCurrentRace = eRaceType::RACE_ZERG;

    m_iBirthStart = 0;
    m_iBirthEnd = 7;

    // 유닛 타입 설정
    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::LAVA;

    // 프레임 정보 초기화
    m_tFrame.iFrame = 0;        // 현재 방향 (0~15)
    m_tFrame.iStart = 0;        // 현재 프레임 (0~4)
    m_tFrame.iEnd = 4;          // 마지막 프레임
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = GetTickCount();
    m_tFrame.dwSpeed = 80;      // 빠른 애니메이션 (80ms)
}

int CLava::Update()
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
        // 방향 설정
        m_tFrame.iFrame = DirTo16WayIndex(m_vDir);
        // 애니메이션 진행
        if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
        {
            m_tFrame.iStart++;
            if (m_tFrame.iStart > m_tFrame.iEnd)
                m_tFrame.iStart = 0;
            m_tFrame.dwTime = now;
        }
        break;
    case eUnitState::ATTACK:
        break;
    case eUnitState::BIRTH:
        //Birth 진행
        if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
        {
            m_iBirthStart++;
            if (m_iBirthStart >= m_iBirthEnd)
            {
                //m_iBirthStart = m_iBirthEnd;
                CompleteBirth();
                return DEAD;
                //m_eState = eUnitState::DIE;
            }
            m_tFrame.dwTime = now;
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

void CLava::Late_Update()
{
}

void CLava::Render(HDC hDC)
{
    // 전장의 안개
    CUnit::Render(hDC);

    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    //상태에 따른 다른 키 생성
    TCHAR szKey[256];
    switch (m_eState)
    {
    case eUnitState::IDLE:
    case eUnitState::MOVE:
        wsprintf(szKey, L"Lava_Move%d_Anim%d", m_tFrame.iFrame, m_tFrame.iStart);
        break;
    case eUnitState::BIRTH:
        wsprintf(szKey, L"Lava_Birth%d", m_iBirthStart);
        break;
    case eUnitState::DIE:
        break;
    default:
        wsprintf(szKey, L"Lava_Move%d_Anim%d", m_tFrame.iFrame, m_tFrame.iStart);
        break;
    }

    // 이미지 가져오기
    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szKey);
    if (pPng)
    {
        int iWidth = pPng->Get_Width();
        int iHeight = pPng->Get_Height();

        pPng->Render_Alpha(hDC,
            (int)m_tInfo.fX - iScrollX - iWidth / 2,
            (int)m_tInfo.fY - iScrollY - iHeight / 2, iWidth, iHeight, false);
    }
}

void CLava::Release()
{
}

void CLava::UpdateHotKeys()
{
    CUnit::UpdateHotKeys();
}

bool CLava::ExecuteCommand(eCommandID command, CommandContext& context)
{
    ResourceCost cost{};
    //먼저 부모 명령 실행
    if (CUnit::ExecuteCommand(command, context))
        return true;

    // CCommandMgr로 위임, 부모 명령 실행 이후 진행
    switch (command)
    {
    case eCommandID::DRONE:
        StartBirth(eUnitType::DRONE);
        return true;
    case eCommandID::OVERLOAD:
        StartBirth(eUnitType::OVERLOAD);
        return true;
    case eCommandID::ZERGLING:
        StartBirth(eUnitType::ZERGLING);
        return true;
    case eCommandID::HYDRALISK:
        StartBirth(eUnitType::HYDRALISK);
        return true;
    case eCommandID::MUTALISK:
        StartBirth(eUnitType::MUTALISK);
        return true;
    case eCommandID::SCOURGE:
        StartBirth(eUnitType::ZERGLING);
        return true;
    case eCommandID::QUEEN:
        StartBirth(eUnitType::ZERGLING);
        return true;
    case eCommandID::DEFILER:
        StartBirth(eUnitType::ZERGLING);
        return true;
    case eCommandID::ULTRALISK:
        StartBirth(eUnitType::ULTRALISK);
        return true;
    }
    return false;
}

void CLava::CommandCardSlot(vector<CommandSlot>& outSlot)
{
    //CONSTRUCTING 중에는 건설 슬롯 안 보이게
    if (m_eState == eUnitState::CONSTRUCTING)
        return;
    //건설 모드 체크 CommandMgr로 변경
    if (CCommandMgr::Get_Instance()->IsPlacing())
        return;

    bool bSpawningPool = CObjMgr::Get_Instance()->IsSpawningPoolBuilt();
    bool bHydraliskDen = CObjMgr::Get_Instance()->IsHydraliskDenBuilt();
    bool bSpire = CObjMgr::Get_Instance()->IsSpireBuilt();
    bool bUltraliskDen = CObjMgr::Get_Instance()->IsUltraliskDenBuilt();

    outSlot.clear();
    outSlot.resize(9);
    for (int i = 0; i < 9; ++i)
    {
        outSlot[i].slotIndex = i;
        outSlot[i].commandID = eCommandID::NONE;
        outSlot[i].iconKey = TEXT("");
        outSlot[i].hotkey = 0;
        outSlot[i].clickable = false;
        outSlot[i].visible = false;
    }
    //0번 : Drone 생성
    outSlot[0].commandID = eCommandID::DRONE;
    outSlot[0].iconKey = TEXT("ICON_COMMAND_CENTER");
    outSlot[0].hotkey = 'W';
    outSlot[0].clickable = true;
    outSlot[0].visible = true;
    //1번 : Overload 생성
    outSlot[1].commandID = eCommandID::OVERLOAD;
    outSlot[1].iconKey = TEXT("ICON_SUPPLY_DEPOT");
    outSlot[1].hotkey = 'E';
    outSlot[1].clickable = true;
    outSlot[1].visible = true;
    //2번 : Zergling 생성
    outSlot[2].commandID = eCommandID::ZERGLING;
    outSlot[2].iconKey = TEXT("ICON_REFINARY");
    outSlot[2].hotkey = 'R';
    outSlot[2].clickable = true;
    outSlot[2].visible = true;
    outSlot[2].lock = !bSpawningPool;
    //3번 : Hydralisk 생성
    outSlot[3].commandID = eCommandID::HYDRALISK;
    outSlot[3].iconKey = TEXT("ICON_BARRACKS");
    outSlot[3].hotkey = 'S';
    outSlot[3].clickable = true;
    outSlot[3].visible = true;
    outSlot[3].lock = !bHydraliskDen;
    //4번 : Mutalisk 생성
    outSlot[4].commandID = eCommandID::MUTALISK;
    outSlot[4].iconKey = TEXT("ICON_ENGINEERING_BAY");
    outSlot[4].hotkey = 'D';
    outSlot[4].clickable = true;
    outSlot[4].visible = true;
    outSlot[4].lock = !bSpire;
    //5번 : Scourge 생성
    outSlot[5].commandID = eCommandID::SCOURGE;
    outSlot[5].iconKey = TEXT("ICON_ENGINEERING_BAY");
    outSlot[5].hotkey = 'F';
    outSlot[5].clickable = true;
    outSlot[5].visible = true;
    outSlot[5].lock = true;
    //6번 : Queen 생성
    outSlot[6].commandID = eCommandID::QUEEN;
    outSlot[6].iconKey = TEXT("ICON_ACADEMY");
    outSlot[6].hotkey = 'X';
    outSlot[6].clickable = true;
    outSlot[6].visible = true;
    outSlot[6].lock = true;
    //7번 : Defiler 생성
    outSlot[7].commandID = eCommandID::DEFILER;
    outSlot[7].iconKey = TEXT("ICON_ACADEMY");
    outSlot[7].hotkey = 'C';
    outSlot[7].clickable = true;
    outSlot[7].visible = true;
    outSlot[7].lock = true;
    //8번 : Ultralisk 생성
    outSlot[8].commandID = eCommandID::ULTRALISK;
    outSlot[8].iconKey = TEXT("ICON_ACADEMY");
    outSlot[8].hotkey = 'V';
    outSlot[8].clickable = true;
    outSlot[8].visible = true;
    outSlot[8].lock = !bUltraliskDen;
}

void CLava::UpdateDead()
{
    //이펙트와 사운드 재생
    CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingDeath.wav", 0.5f);

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

void CLava::StartBirth(eUnitType type)
{
    m_eBirthUnit = type;
    m_eState = eUnitState::BIRTH;
    m_bBirthing = true;
}

void CLava::CompleteBirth()
{
    m_bBirthing = false;
    CObj* pUnit = nullptr;
    CObj* pZergling = nullptr;
    Vec2 pos = Get_Pos();

    switch (m_eBirthUnit)
    {
    case eUnitType::DRONE:
        CSoundMgr::Get_Instance()->PlayEffect(L"Drone/DroneBirth.wav", 0.5f);
        pUnit = CAbstractFactory<CDrone>::Create(pos.fX, pos.fY);
        CResourceMgr::Get_Instance()->AddSupply(1);
        break;
    case eUnitType::OVERLOAD:
        CSoundMgr::Get_Instance()->PlayEffect(L"Overload/zovRdy00.wav", 0.5f);
        pUnit = CAbstractFactory<COverload>::Create(pos.fX, pos.fY);
        CResourceMgr::Get_Instance()->AddDepotSupply(10);
        break;
    case eUnitType::ZERGLING:
        CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingBirth.wav", 0.5f);
        pUnit = CAbstractFactory<CZergling>::Create(pos.fX, pos.fY);
        pZergling = CAbstractFactory<CZergling>::Create(pos.fX, pos.fY + 10.f);
        CResourceMgr::Get_Instance()->AddSupply(2);
        break;
    case eUnitType::HYDRALISK:
        CSoundMgr::Get_Instance()->PlayEffect(L"Hydralisk/HydraliskBirth.wav", 0.5f);
        pUnit = CAbstractFactory<CHydralisk>::Create(pos.fX, pos.fY);
        CResourceMgr::Get_Instance()->AddSupply(1);
        break;      
    case eUnitType::MUTALISK:
        CSoundMgr::Get_Instance()->PlayEffect(L"Mutalisk/MutaliskBirth.wav", 0.5f);
        pUnit = CAbstractFactory<CMutalisk>::Create(pos.fX, pos.fY);
        CResourceMgr::Get_Instance()->AddSupply(1);
        break;
    case eUnitType::ULTRALISK:
        CSoundMgr::Get_Instance()->PlayEffect(L"Ultralisk/UltraliskBirth.wav", 0.5f);
        pUnit = CAbstractFactory<CUltralisk>::Create(pos.fX, pos.fY);
        CResourceMgr::Get_Instance()->AddSupply(2);
        break;
    default:
        break;
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pUnit);
    if (pZergling)
    {
        CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pZergling);
    }
}
