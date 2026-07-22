#include "pch.h"
#include "CUIMgr.h"
#include "CBmpMgr.h"
#include "CMainUI.h"
#include "Commandable.h"
#include "CSelectionMgr.h"
#include "CTimeMgr.h"
#include "CUnit.h"
#include "CBuilding.h"

CUIMgr* CUIMgr::m_pInstance = nullptr;

CUIMgr::CUIMgr() {}

CUIMgr::~CUIMgr()
{
    Release();
}

CUIMgr* CUIMgr::Get_Instance()
{
    if (!m_pInstance)
        m_pInstance = new CUIMgr;
    return m_pInstance;
}

void CUIMgr::Destroy_Instance()
{
    if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

void CUIMgr::Initialize()
{
    //게임 데이터 저장
    m_eRaceType = CGameDataMgr::Get_Instance()->Get_PlayerRace();

    // MainUI.bmp를 한 번만 로드
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MainUI/MainUI.bmp", L"MainUI");
    //MainUI - Menu 버튼
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/MenuBtn.bmp", L"MENU_BTN");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/MenuBtnOn.bmp", L"MENU_BTN_ON");
    //UIButton 로드
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/ButtonIcon.bmp", L"ICON_BUTTON_UI");
    //건물  
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/CommandCenter/CommandCenter.bmp", L"COMMANDCENTER_GREEN");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/CommandCenter/CommandCenter_Red.bmp", L"COMMANDCENTER_RED");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Barrack/Barrack.bmp", L"BARRACK_GREEN");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Barrack/Barrack_Red.bmp", L"BARRACK_RED");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/SupplyDepot/SupplyDepot.bmp", L"SUPPLYDEPOT_GREEN");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/SupplyDepot/SupplyDepot_Red.bmp", L"SUPPLYDEPOT_RED");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Starport/Starport.bmp", L"STARPORT_GREEN");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Starport/Starport_Red.bmp", L"STARPORT_RED");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Factory/Factory.bmp", L"FACTORY_GREEN");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Factory/Factory_Red.bmp", L"FACTORY_RED");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Refinery/Refinery4.bmp", L"REFINERY_GREEN");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Refinery/Refinery_Red.bmp", L"REFINERY_RED");
    //알파마스크
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/trash/Structure/BuildAlpha.bmp", L"BUILD_ALPAH");
    //ProgressUI
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/HpRect.bmp", L"HP_RECT");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/MpRect.bmp", L"MP_RECT");
    //CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/MpRect.png", L"MP_RECT");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/UIRect.bmp", L"UI_RECT");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/ProgressEmpty.bmp", L"PROGRESS_EMPTY");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/ProgressFull.bmp", L"PROGRESS_FULL");
    //Wire 이미지
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Wire/BigWire.bmp", L"WIRE");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Wire/SmallWire.bmp", L"SMALL_WIRE");
    //유닛 Spawn 이미지(SCV, 마린, 파이어벳, 고스트, 벌쳐, 시즈탱크, 골리앗)
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/UnitSpawn_Icon/SpawnIcon.bmp", L"SPAWN_ICON");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/MiniMap.bmp", L"MINIMAP");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Map/MiniMapFrame.bmp", L"MINIMAP_FRAME");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/ETC.bmp", L"ETC");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/UpgradeDetail2.bmp", L"TRAIN_FRAME");
    //유닛 Armor, Attack Icon
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/ArmorIcon.bmp", L"ARMOR_ICON");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/SCVAttackIcon.bmp", L"SCV_ATTACK_ICON");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/MarineAttackIcon.bmp", L"MARINE_ATTACK_ICON");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/VultureAttackIcon.bmp", L"VULTURE_ATTACK_ICON");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/TankAttackIcon.bmp", L"TANK_ATTACK_ICON");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/SiegeTankAttackIcon.bmp", L"SIEGETANK_ATTACK_ICON");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/FirebatAttackIcon.bmp", L"FIREBAT_ATTACK_ICON");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/BattleCrusorAttackIcon1.bmp", L"BATTLECRUISER_ATTACK_ICON");
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/GhostAttackIcon.bmp", L"GHOST_ATTACK_ICON");
    //CMainUI 생성 및 초기화 (여기서 알파 비트맵 생성됨)
    CMainUI::Get_Instance()->Initialize();
}

int CUIMgr::Update()
{
    //MainUI쪽 Button 업데이트
    CMainUI::Get_Instance()->Update();

    //버튼 피드백 타이머 업데이트
    if (m_iActiveFeedbackSlot >= 0)
    {
        float dt = CTimeMgr::Get_Instance()->GetDT();
        m_fFeedbackTimer -= dt;
        if (m_fFeedbackTimer < 0.f)
        {
            m_iActiveFeedbackSlot = -1;
            m_fFeedbackTimer = 0.f;
        }
    }
    //UIInfo 정보 업데이트
    UpdateUIInfoFromSelection();

    return 0;
}

void CUIMgr::Release()
{
    CMainUI::Get_Instance()->Destroy_Instance();
}

void CUIMgr::Render(HDC hDC)
{
    //1. 배경 렌더링
    CMainUI::Get_Instance()->Render(hDC);
    //선택된 오브젝트 CommandSlot 렌더링
    RenderSelectedCommandSlots(hDC);
}

void CUIMgr::UpdateUIInfoFromSelection()
{
    auto& sel = CSelectionMgr::Get_Instance()->GetSelected();

    if (sel.size() == 1 && sel[0])
    {
        if (auto* u = dynamic_cast<CUnit*>(sel[0]))
        {
            u->UpdateUnitUIInfo();
            // 건물 UI는 숨김
            BuildingUIInfo bi{}; bi.IsVisible = false;
            CMainUI::Get_Instance()->SetBuildingUIInfo(bi);
            return;
        }
        if (auto* b = dynamic_cast<CBuilding*>(sel[0]))
        {
            b->UpdateBuildingUIInfo();
            // 유닛 UI는 숨김
            UnitUIInfo ui{}; ui.IsVisible = false;
            CMainUI::Get_Instance()->SetUnitUIInfo(ui);
            return;
        }
    }
    // 선택 0개 or 다중선택이면 단일 UI 숨김(다중선택 UI는 별도 처리)
    UnitUIInfo ui{}; ui.IsVisible = false;
    CMainUI::Get_Instance()->SetUnitUIInfo(ui);

    BuildingUIInfo bi{}; bi.IsVisible = false;
    CMainUI::Get_Instance()->SetBuildingUIInfo(bi);
}

void CUIMgr::LoadProtossResource()
{
}

void CUIMgr::LoadZergResource()
{
}

void CUIMgr::RenderSelectedCommandSlots(HDC hDC)
{
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
    //선택된 obj가 존재하지 않을 경우 return
    if (selected.size() == 0)
        return;

    //선택된 obj가 하나일 경우 commandslot 렌더링
    if (selected.size() == 1)
    {
        CObj* pObj = selected[0];
        //적군 체크! 
        eTeamType type = pObj->GetTeamType();
        if (type == eTeamType::ENEMY)
            return;
        //Commandable 인터페이스 여부 확인
        Commandable* commandable = dynamic_cast<Commandable*>(pObj);
        if (commandable)
        {
            vector<CommandSlot> slots;
            commandable->CommandCardSlot(slots);
            //커맨드 카드 렌더링 위치
            //실제 레이아웃에 맞게 조정
            int baseX = 638;
            int baseY = 450;
            eRaceType raceType = pObj->GetOriginalRace();
            RenderCommandSlots(hDC, slots, baseX, baseY, raceType);
        }
        return;
    }

    //다중 선택 - 유닛
    CUnit* pFirstUnit = dynamic_cast<CUnit*>(selected[0]);
    if (pFirstUnit && !pFirstUnit->IsDead()) //죽은 유닛에 대한 체크 추가!!
    {
        //적군 체크! 
        eTeamType teamType = pFirstUnit->GetTeamType();
        if (teamType == eTeamType::ENEMY)
            return;
        eUnitType unitType = pFirstUnit->Get_UnitType();
        //모두 "동일한 타입"의 "유닛"인지 판단
        for (auto* obj : selected)
        {
            CUnit* pUnit = dynamic_cast<CUnit*>(obj);
            //유닛이 아니거나 유닛 타입이 다르거나 죽었을 경우 return!
            if (!pUnit || pUnit->Get_UnitType() != unitType || pUnit->IsDead())
                return;
        }
        //첫번째 유닛의 커맨드 카드 슬롯 렌더링(모두 동일 타입의 유닛 판단 완료!)
        Commandable* cmd = dynamic_cast<Commandable*>(pFirstUnit);
        if (cmd)
        {
            vector<CommandSlot> slots;
            cmd->CommandCardSlot(slots);
            //커맨드 카드 렌더링 위치
            int baseX = 638;
            int baseY = 450;
            eRaceType raceType = pFirstUnit->GetOriginalRace();
            RenderCommandSlots(hDC, slots, baseX, baseY, raceType);
        }
        return;
    }

    //다중 선택 - 유닛 아니면 건물
    CBuilding* pFirstBuilding = dynamic_cast<CBuilding*>(selected[0]);
    if (pFirstBuilding)
    {
        //적군 체크! 
        eTeamType teamType = pFirstBuilding->GetTeamType();
        if (teamType == eTeamType::ENEMY)
            return;
        eBuildingType buildingType = pFirstBuilding->GetBuildingType();
        //모두 동일한 타입의 건물인지 판단
        for (auto* obj : selected)
        {
            CBuilding* pBuilding = dynamic_cast<CBuilding*>(obj);
            //건물이 아니거나 건물 타입이 다를 경우 return
            if (!pBuilding || pBuilding->GetBuildingType() != buildingType)
                return;
        }
        //첫번째 건물의 커맨드 카드 슬롯 렌더링
        Commandable* cmd = dynamic_cast<Commandable*>(pFirstBuilding);
        if (cmd)
        {
            vector<CommandSlot> slots;
            cmd->CommandCardSlot(slots);
            //커맨드 카드 렌더링 위치
            int baseX = 638;
            int baseY = 450;
            eRaceType raceType = pFirstBuilding->GetOriginalRace();
            RenderCommandSlots(hDC, slots, baseX, baseY, raceType);
        }
        return;
    }
}

void CUIMgr::RenderCommandSlots(HDC hDC, vector<CommandSlot>& slots, 
    int baseX, int baseY, eRaceType type)
{
    switch (type)
    {
    case eRaceType::RACE_TERRAN:
        RenderCommandSlots_Terran(hDC, slots, baseX, baseY);
        break;
    case eRaceType::RACE_ZERG:
        RenderCommandSlots_Zerg(hDC, slots, baseX, baseY);
        break;
    case eRaceType::RACE_PROTOSS:
        RenderCommandSlots_Protoss(hDC, slots, baseX, baseY);
        break;
    default:
        break;
    }
}

void CUIMgr::RenderIcon_Terran(HDC hDC, eIconState eState, int iconIndex, int destX, int destY)
{
    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"ICON_BUTTON_UI");
    if (!hMemDC) return;
    //소스 영역 계산
    RECT srcRect = GetIconRect(iconIndex, eState);

    GdiTransparentBlt(hDC,
        destX, destY,
        ICON_WIDTH, ICON_HEIGHT,
        hMemDC,
        srcRect.left,
        srcRect.top,
        ICON_WIDTH,
        ICON_HEIGHT,
        RGB(0, 255, 0));
}

void CUIMgr::RenderCommandSlots_Terran(HDC hDC, vector<CommandSlot>& slots, int baseX, int baseY)
{
    //9개 슬롯을 3X3의 그리드로 렌더링
    for (int i = 0; i < 9; ++i)
    {
        //슬롯 범위 체크
        if (i >= (int)slots.size())
            break;
        //보이지 않는 슬롯 스킵
        if (!slots[i].visible)
            continue;
        //아이콘 인덱스 가져오기
        int iconIndex = GetIconIndex(slots[i].commandID);
        if (iconIndex < 0)
            continue;
        eIconState state = eIconState::YELLOW;
        if (m_iActiveFeedbackSlot == i)
        {
            //상태 결정
            state = eIconState::WHITE;
        }
        else if (slots[i].lock)
            state = eIconState::GRAY; //클릭 불가능 - 회색
        //자원 부족, 다른 조건 체크 추가
        int col = i % 3;
        int row = i / 3;

        int x = baseX + col * (ICON_WIDTH + SLOT_GAP_X);
        int y = baseY + row * (ICON_HEIGHT + SLOT_GAP_Y);
        //아이콘 렌더링
        RenderIcon_Terran(hDC, state, iconIndex, x, y);
        //TODO: 단축키 텍스트 렌더링하기
    }
}

void CUIMgr::RenderCommandSlots_Protoss(HDC hDC, vector<CommandSlot>& slots, int baseX, int baseY)
{
    //9개 슬롯을 3X3의 그리드로 렌더링
    for (int i = 0; i < 9; ++i)
    {
        //슬롯 범위 체크
        if (i >= (int)slots.size())
            break;
        //보이지 않는 슬롯 스킵
        if (!slots[i].visible)
            continue;
        //아이콘 상태 결졍
        eIconState state = eIconState::YELLOW;
        if (m_iActiveFeedbackSlot == i)
        {
            state = eIconState::WHITE;
        }
        else if (slots[i].lock)
        {
            state = eIconState::WHITE;
        }
        else if (!slots[i].lock)
        {
            state = eIconState::YELLOW;
        }
        //자원 부족, 다른 조건 체크 추가
        int col = i % 3;
        int row = i / 3;

        int x = baseX + col * (ICON_WIDTH + SLOT_GAP_X);
        int y = baseY + row * (ICON_HEIGHT + SLOT_GAP_Y);
        //아이콘 렌더링
        RenderIcon_Protoss(hDC, slots[i].commandID, state, x, y);
        //TODO: 단축키 텍스트 렌더링하기
    }
}

void CUIMgr::RenderIcon_Protoss(HDC hDC, eCommandID commandID ,eIconState state, int destX, int destY)
{
    //이미지 키 가져오기
    const TCHAR* imageKey = GetIcon_Protoss(commandID, state);
    if (!imageKey)
        return;
    //이미지 찾고 출력하기
    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(imageKey);
    if (!pPng) return;
    int width = pPng->Get_Width();
    int height = pPng->Get_Height();
    pPng->Render_Alpha(hDC, destX, destY,
        width, height, false);
}

const TCHAR* CUIMgr::GetIcon_Protoss(eCommandID commandId, eIconState state)
{
    wstring name;

    switch (commandId)
    {
        //유닛
    case eCommandID::PROBE:
        name = L"Probe";
        break;
    case eCommandID::ZEALOT:
        name = L"Zealot";
        break;
    case eCommandID::DRAGOON:
        name = L"Dragoon";
        break;
    case eCommandID::HIGH_TEMPLAR:
        name = L"HighTemplar";
        break;
    case eCommandID::DARK_TEMPLAR:
        name = L"DarkTemplar";
        break;
    case eCommandID::OBSERVER:
        name = L"Observer";
        break;
    case eCommandID::SHUTTLE:
        name = L"Shuttle";
        break;
    case eCommandID::REAVER:
        name = L"Reaver";
        break;
    case eCommandID::CORSAIR:
        name = L"Corsair";
        break;
    case eCommandID::SCOUT:
        name = L"Scout";
        break;
    case eCommandID::CARRIER:
        name = L"Carrier";
        break;
    case eCommandID::ARBITER:
        name = L"Arbiter";
        break;
    case eCommandID::MOVE:
        name = L"Move";
        break;
    case eCommandID::STOP:
        name = L"Stop";
        break;
    case eCommandID::HOLD:
        name = L"Hold";
        break;
    case eCommandID::PATROL:
        name = L"Patrol";
        break;
    case eCommandID::ATTACK:
        name = L"Attack";
        break;
    case eCommandID::RALLY:
        name = L"Rally";
        break;
    case eCommandID::CANCLE:
        name = L"Cancle";
        break;
    case eCommandID::NORMAL_BUILDING:
        name = L"LBuild";
        break;
    case eCommandID::ADVANCED_BUILDING:
        name = L"ABuild";
        break;
    case eCommandID::NEXUS:
        name = L"Nexus";
        break;
    case eCommandID::PYLON:
        name = L"Pylon";
        break;
    case eCommandID::ASSIMILATOR:
        name = L"Assimilator";
        break;
    case eCommandID::GATEWAY:
        name = L"Gateway";
        break;
    case eCommandID::FORGE:
        name = L"Forge";
        break;
    case eCommandID::PHOTON_CANNON:
        name = L"Photon_Cannon";
        break;
    case eCommandID::CYBERNETICS_CORE:
        name = L"Cybernetics_Core";
        break;
    case eCommandID::SHIELD_BATTERY:
        name = L"Shield_Battery";
        break;
        //ABuilding
    case eCommandID::ROBOTICS_FACILITY:
        name = L"Robotics_Facility";
        break;
    case eCommandID::STARGATE:
        name = L"Stargate";
        break;
    case eCommandID::CITADEL_OF_ADUN:
        name = L"CitadelOfAdun";
        break;
    case eCommandID::ROBOTICS_SUPPORT_BAY:
        name = L"RoboticsSupportBay";
        break;
    case eCommandID::FLEET_BEACON:
        name = L"Fleet_Beacon";
        break;
    case eCommandID::TEMPLAR_ARCHIVES:
        name = L"Templar_Archives";
        break;
    case eCommandID::OBSERVATORY:
        name = L"Observatory";
        break;
    case eCommandID::ARBITER_TRIBUNAL:
        name = L"Arbiter_Tribunal";
        break;
    case eCommandID::RUSH:
        name = L"Rush";
        break;
    case eCommandID::BLINK:
        name = L"Blink";
        break;
    case eCommandID::MIND_CONTROL:
        name = L"MindControl";
        break;
    case eCommandID::FEEDBACK:
        name = L"Feedback";
        break;
    case eCommandID::MAELSTROM:
        name = L"Maelstrom";
        break;
    case eCommandID::STORM:
        name = L"Storm";
        break;
    case eCommandID::HALLUCINATION:
        name = L"Hallucination";
        break;
    case eCommandID::WRAP:
        name = L"Wrap";
        break;
    case eCommandID::INTERCEPTOR:
        name = L"Interceptor";
        break;
    case eCommandID::RECALL:
        name = L"Recall";
        break;
    case eCommandID::ICE:
        name = L"Ice";
        break;
    case eCommandID::LOADING:
        name = L"Loading";
        break;
    case eCommandID::LANDING:
        name = L"Landing";
        break;
    default:
        return nullptr;
    }

    int suffix = static_cast<int>(state);

    //최종 키 생성하기
    static wstring result;
    result = name + L"_" + to_wstring(suffix);
    return result.c_str();
}

void CUIMgr::RenderIcon_Zerg(HDC hDC, eCommandID commandID ,eIconState eState, int destX, int destY)
{
    //이미지 키 가져오기
    const TCHAR* imageKey = GetIcon_Zerg(commandID, eState);
    if (!imageKey)
        return;
    //이미지 찾고 출력하기
    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(imageKey);
    if (!pPng) return;
    int width = pPng->Get_Width();
    int height = pPng->Get_Height();
    pPng->Render_Alpha(hDC, destX, destY,
        width, height, false);
}

void CUIMgr::RenderCommandSlots_Zerg(HDC hDC, vector<CommandSlot>& slots, int baseX, int baseY)
{
    //9개 슬롯을 3X3의 그리드로 렌더링
    for (int i = 0; i < 9; ++i)
    {
        //슬롯 범위 체크
        if (i >= (int)slots.size())
            break;
        //보이지 않는 슬롯 스킵
        if (!slots[i].visible)
            continue;
        //아이콘 상태 결졍
        eIconState state = eIconState::YELLOW;
        if (m_iActiveFeedbackSlot == i)
        {
            state = eIconState::WHITE;
        }
        else if (slots[i].lock)
        {
            state = eIconState::WHITE;
        }
        else if (!slots[i].lock)
        {
            state = eIconState::YELLOW;
        }
        //자원 부족, 다른 조건 체크 추가
        int col = i % 3;
        int row = i / 3;

        int x = baseX + col * (ICON_WIDTH + SLOT_GAP_X);
        int y = baseY + row * (ICON_HEIGHT + SLOT_GAP_Y);
        //아이콘 렌더링
        RenderIcon_Zerg(hDC, slots[i].commandID, state, x, y);
        //TODO: 단축키 텍스트 렌더링하기
    }
}

const TCHAR* CUIMgr::GetIcon_Zerg(eCommandID commandID, eIconState state)
{
    wstring name;

    switch (commandID)
    {
        //유닛
    case eCommandID::LAVA:
        name = L"Zerg_Build17";
        break;
    case eCommandID::DRONE:
        name = L"Zerg_Ground4";
        break;
    case eCommandID::OVERLOAD:
        name = L"Zerg_Air0";
        break;
    case eCommandID::ZERGLING:
        name = L"Zerg_Ground0";
        break;
    case eCommandID::HYDRALISK:
        name = L"Zerg_Ground1";
        break;
    case eCommandID::MUTALISK:
        name = L"Zerg_Air1";
        break;
    case eCommandID::SCOURGE:
        name = L"Zerg_Air2";
        break;
    case eCommandID::QUEEN:
        name = L"Zerg_Air3";
        break;
    case eCommandID::ULTRALISK:
        name = L"Zerg_Ground2";
        break;
    case eCommandID::DEFILER:
        name = L"Zerg_Ground5";
        break;
        //Active 커맨드
    case eCommandID::MOVE:
        name = L"Zerg_Active0";
        break;
    case eCommandID::STOP:
        name = L"Zerg_Active1";
        break;
    case eCommandID::HOLD:
        name = L"Zerg_Active13";
        break;
    case eCommandID::PATROL:
        name = L"Zerg_Active12";
        break;
    case eCommandID::ATTACK:
        name = L"Zerg_Active2";
        break;
    case eCommandID::RALLY:
        name = L"Zerg_Active10";
        break;
    case eCommandID::CANCLE:
        name = L"Zerg_Active6";
        break;
    case eCommandID::NORMAL_BUILDING:
        name = L"Zerg_Build18";
        break;
    case eCommandID::ADVANCED_BUILDING:
        name = L"Zerg_Build19";
        break;
        //LBuild
    case eCommandID::HATCHERY:
        name = L"Zerg_Build0";
        break;
    case eCommandID::LAIR:
        name = L"Zerg_Build15";
        break;
    case eCommandID::HIVE:
        name = L"Zerg_Build3";
        break;
    case eCommandID::COLONY:
        name = L"Zerg_Build13";
        break;
    case eCommandID::EXTRACTOR:
        name = L"Zerg_Build16";
        break;
    case eCommandID::SPAWNING_POOL:
        name = L"Zerg_Build12";
        break;
    case eCommandID::EVOLUTION_CHAMBER:
        name = L"Zerg_Build9";
        break;
    case eCommandID::HYDRALISK_DEN:
        name = L"Zerg_Build5";
        break;
        //ABuild
    case eCommandID::SPIRE:
        name = L"Zerg_Build11";
        break;
    case eCommandID::BASE_QUEEN:
        name = L"Zerg_Build8";
        break;
    case eCommandID::BASE_TELPO:
        name = L"Zerg_Build4";
        break;
    case eCommandID::BASE_ULTRALISK:
        name = L"Zerg_Build10";
        break;
    case eCommandID::BASE_DEFILER:
        name = L"Zerg_Build6";
        break;
    default:
        return nullptr;
    }

    //최종 키 생성하기
    static wstring result;

    if(state != eIconState::YELLOW)
    {
        result = name + L"_0";
    }
    else
    {
        result = name;
    }

    return result.c_str();
}

RECT CUIMgr::GetIconRect(int iconIndex, eIconState eState)
{
    RECT rc{};
    //iconIndex를 통해서 몇 번 째 열인지 계산
    int row = iconIndex;
    int col = static_cast<int>(eState);

    //소스 영역 계산
    rc.left = col * ICON_WIDTH;
    rc.top = row * ICON_HEIGHT;
    rc.right = rc.left + ICON_WIDTH;
    rc.bottom = rc.top + ICON_HEIGHT;

    return rc;
}

void CUIMgr::SetButtonFeedback(int slotIndex, bool pressed)
{
    //버튼이 눌렸을 경우 해당 버튼의 WHITE 이미지 일정 시간 보여주기
    if (pressed)
    {
        m_iActiveFeedbackSlot = slotIndex;
        m_fFeedbackTimer = FEEDBACK_DURATION;
    }
    else
    {
        if (m_iActiveFeedbackSlot == slotIndex)
        {
            m_iActiveFeedbackSlot = -1;
            m_fFeedbackTimer = 0.f;
        }
    }
}

int CUIMgr::GetIconIndex(eCommandID commandID)
{
    switch (commandID)
    {
        // 기본 커맨드
    case eCommandID::CANCLE:
        return IconIndex::CANCLE;
    case eCommandID::MOVE:
        return IconIndex::MOVE;
    case eCommandID::STOP:
        return IconIndex::STOP;
    case eCommandID::ATTACK:
        return IconIndex::ATTACK;
    case eCommandID::PATROL:
        return IconIndex::PATROL;
    case eCommandID::HOLD:
        return IconIndex::HOLD;
    case eCommandID::RALLY:
        return IconIndex::RALLY;
    case eCommandID::NORMAL_BUILDING:
        return IconIndex::NORMAL_BUILDING;
    case eCommandID::ADVANCED_BUILDING:
        return IconIndex::ADVANCED_BUILDING;

        // 유닛 생산
    case eCommandID::SCV:
        return IconIndex::SCV;
    case eCommandID::MARINE:
        return IconIndex::MARINE;
    case eCommandID::MEDIC:
        return IconIndex::MEDIC;
    case eCommandID::FIREBAT:
        return IconIndex::FIREBAT;
    case eCommandID::GHOST:
        return IconIndex::GHOST;
    case eCommandID::VULTURE:
        return IconIndex::VULTURE;
    case eCommandID::TANK:
        return IconIndex::TANK;
    case eCommandID::SIEGE_TANK:
        return IconIndex::SIEGE_TANK;
    case eCommandID::GOLIATH:
        return IconIndex::GOLIATH;
    case eCommandID::BATTLECRUISER:
        return IconIndex::BATTLECRUISER;
        // 건물 건설
    case eCommandID::COMMAND_CENTER:
        return IconIndex::COMMAND_CENTER;
    case eCommandID::SUPPLY_DEPOT:
        return IconIndex::SUPPLY_DEPOT;
    case eCommandID::REFINERY:
        return IconIndex::REFINERY;
    case eCommandID::BARRACKS:
        return IconIndex::BARRACKS;
    case eCommandID::ENGINEERING_BAY:
        return IconIndex::ENGINEERING_BAY;
    case eCommandID::TURRET:
        return IconIndex::TURRET;
    case eCommandID::ACADEMY:
        return IconIndex::ACADEMY;
    case eCommandID::BUNKER:
        return IconIndex::BUNKER;
    case eCommandID::FACTORY:
        return IconIndex::FACTORY;
    case eCommandID::FACTORY_ADDON:
        return IconIndex::FACTORY_ADDON;
    case eCommandID::STARPORT:
        return IconIndex::STARPORT;
    case eCommandID::STARPORT_ADDON:
        return IconIndex::STARPORT_ADDON;
    case eCommandID::SCIENCE_FACILITY:
        return IconIndex::SCIENCE_FACILITY;
    case eCommandID::ARMORY:
        return IconIndex::ARMORY;
    case eCommandID::COMBAT_STATION:
        return IconIndex::COMBAT_STATION;
    case eCommandID::NUCLEAR_SILO:
        return IconIndex::NUCLEAR_SILO;
    case eCommandID::SCIENCE_SECRET:
        return IconIndex::SCIENCE_SECRET;
    case eCommandID::SCIENCE_PHYSICS:
        return IconIndex::SCIENCE_PHYSICS;
    //유틸리티
    case eCommandID::U238:
        return IconIndex::U238;
    case eCommandID::STEAMPACK:
        return IconIndex::STEAMPACK;
    case eCommandID::RESTORATION:
        return IconIndex::RESTORATION;
    case eCommandID::OPTICAL_FLARE:
        return IconIndex::OPTICAL_FLARE;
    case eCommandID::CADUCEUS_REACTOR:
        return IconIndex::CADUCEUS_REACTOR;
    case eCommandID::MINE:
        return IconIndex::MINE;
    case eCommandID::CLOCKING:
        return IconIndex::CLOCKING;
    case eCommandID::NUCLEAR:
        return IconIndex::NUCLEAR;
    case eCommandID::SCANNER:
        return IconIndex::SCANNER;
    case eCommandID::ATTACK_UPGRADE:
        return IconIndex::ATTACK_UPGRADE;
    case eCommandID::ARMOR_UPGRADE:
        return IconIndex::ARMOR_UPGRADE;
    case eCommandID::VEHICLE_ATTACK_UPGRADE:
        return IconIndex::VEHICLE_ATTACK_UPGRADE;
    case eCommandID::VEHICLE_ARMOR_UPGRADE:
        return IconIndex::VEHICLE_ARMOR_UPGRADE;
    case eCommandID::SHIP_ATTACK_UPGRADE:
        return IconIndex::SHIP_ATTACK_UPGRADE;
    case eCommandID::SHIP_ARMOR_UPGRADE:
        return IconIndex::SHIP_ARMOR_UPGRADE;
    case eCommandID::EMP:
        return IconIndex::EMP;
    case eCommandID::IRRADIATE:
        return IconIndex::IRRADIATE;
    case eCommandID::TITAN:
        return IconIndex::TITAN;
    case eCommandID::YAMATO:
        return IconIndex::YAMATO;
    case eCommandID::COLOSSUS:
        return IconIndex::COLOSSUS;
    case eCommandID::NUCLEAR_MISSILE:
        return IconIndex::NUCLEAR_MISSILE;
        /*
    case eCommandID::FIREBAT:
        return IconIndex::FIREBAT;
    case eCommandID::MEDIC:
        return IconIndex::MEDIC;
    case eCommandID::GHOST:
        return IconIndex::GHOST;

        // 건물 건설
    case eCommandID::COMMAND_CENTER:
        return IconIndex::COMMAND_CENTER;
    case eCommandID::SUPPLY_DEPOT:
        return IconIndex::SUPPLY_DEPOT;
    case eCommandID::BARRACKS:
        return IconIndex::BARRACKS;
    case eCommandID::REFINERY:
        return IconIndex::REFINERY;
    case eCommandID::FACTORY:
        return IconIndex::FACTORY;
    case eCommandID::STARPORT:
        return IconIndex::STARPORT;
        // TODO: 다른 커맨드들 추가
        */
    default:
        return -1;  // 잘못된 커맨드
    }
}


