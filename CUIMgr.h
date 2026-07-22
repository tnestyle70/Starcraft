#pragma once
#include "CGameDataMgr.h"
#include <vector>
class CMainUI;
//전방 선언
struct CommandSlot;
enum class eCommandID;

namespace IconIndex
{
    // 기본 커맨드 (0~)
    const int MOVE = 0;             // 이동 (화살표)
    const int STOP = 1;           // 취소 (X)
    const int ATTACK = 2;           // 순찰 (점선 경로)
    const int PATROL = 3;           // 귀환 (회귀 화살표)
    const int HOLD = 4;             // 홀드 (위치 고수)
    const int CANCLE = 5;             // 정지 (금지)
    const int RALLY = 6;           // 공격 (조준점)
    const int GATHER = 7;           // 자원 채취 (폭발)
    const int REPAIR = 8;          // 수리 (삼각형 렌치)
    const int NORMAL_BUILDING = 9; //일반 건물
    const int ADVANCED_BUILDING = 10; //고급 건물
    // 유닛 아이콘 (10~)
    const int SCV = 11;             // SCV
    const int MARINE = 12; //마린
    const int MEDIC = 13; //메딕
    const int GHOST = 14; //고스트
    const int FIREBAT = 42; //파이어뱃
    const int VULTURE = 15; //벌쳐
    const int TANK = 16; //탱크
    const int GOLIATH = 17; //골리앗
    const int SIEGE_TANK = 40; //시즈 탱크
    const int BATTLECRUISER = 20; //배틀크루저
    //건물 아이콘
    const int COMMAND_CENTER = 22; //커맨드 센터
    const int SUPPLY_DEPOT = 23; //보급고
    const int REFINERY = 24; //정제소 
    const int BARRACKS = 25; //배럭
    const int ENGINEERING_BAY = 26; //엔지니어링베이
    const int TURRET = 27; //터렛
    const int ACADEMY = 28; //아카데미
    const int BUNKER = 29; //벙커
    const int FACTORY = 30; //군수공장
    const int STARPORT = 31; //스타포트
    const int SCIENCE_FACILITY = 32; //과학연구소
    const int ARMORY = 33; //아머리
    const int COMBAT_STATION = 34; //커맨드 센터 - 컴뱃 스테이션
    const int NUCLEAR_SILO = 35; //커맨드 센터 - 뉴클리어 사일로
    const int SCIENCE_SECRET = 36; //과학 연구 시설 - 사이언스 시크릿
    const int SCIENCE_PHYSICS = 37; //과학 연구 시설 - 사이언스 피직스
    const int STARPORT_ADDON = 38; //스타포트 애드온
    const int FACTORY_ADDON = 39; //군수공장 애드온
    //유틸리티 아이콘
    const int U238 = 44; //아카데미 - 마린 사거리
    const int STEAMPACK = 45; //아카데미 - 스팀팩
    const int RESTORATION = 46; //아카데미 - 메딕 회복
    const int OPTICAL_FLARE = 47; //아카데미 - 메딕 광학 섬광탄
    const int CADUCEUS_REACTOR = 48; //아카데미 - 메딕 카두세우스 반응로
    const int ATTACK_UPGRADE = 49; //엔지니어링베이 - 공격력
    const int ARMOR_UPGRADE = 50; //엔지니어링베이 - 방어력
    const int VEHICLE_ATTACK_UPGRADE = 51; //아머리 - 지상 공격력
    const int VEHICLE_ARMOR_UPGRADE = 52; //아머리 - 지상 방어력
    const int SHIP_ATTACK_UPGRADE = 53; //아머리 - 공중 공격력
    const int SHIP_ARMOR_UPGRADE = 54; //아머리 - 공중 방어력
    const int EMP = 55; //과학 연구소 - EMP
    const int IRRADIATE = 56; //과학 연구소 - IRRADIATE
    const int TITAN = 57; //과학 연구소 - TITAN
    const int CLOCKING = 58; //고스트 클로킹
    const int MINE = 62; //벌처 마인
    const int YAMATO = 65; //과학 연구 시설 - 사이언스 피직스 - 야마토
    const int COLOSSUS = 66; //과학 연구 시설 - 사이언스 피직스 - 콜로서스 반응로
    const int NUCLEAR_MISSILE = 67; //뉴클리어 사일로 - 핵미사일
    const int SCANNER = 72; //스캐너
    const int NUCLEAR = 75; //고스트 핵
}

enum class eIconState
{
    YELLOW = 0, //선택 가능한 상태
    WHITE = 1, //선택한 상태
    RED = 2,
    GREEN_BG = 3,
    GRAY = 4,
    GREEN_ICON = 5
};

class CUIMgr
{
private:
    CUIMgr();
    ~CUIMgr();

public:
    static CUIMgr* Get_Instance();
    static void Destroy_Instance();

    void Initialize();
    int Update();
    void Release();
    void Render(HDC hDC);
public:
    void RenderSelectedCommandSlots(HDC hDC); //선택된 유닛 커맨드 슬롯 렌더링
    //커맨드 슬롯 9개 렌더링(배치 렌더링)
    void RenderCommandSlots(HDC hDC, vector<CommandSlot>& slots, int baseX, int baseY, eRaceType type);

    void RenderIcon_Terran(HDC hDC, eIconState eState, int iconIndex, int destX, int destY);
    void RenderCommandSlots_Terran(HDC hDC, vector<CommandSlot>& slots, int baseX, int baseY);

    void RenderIcon_Zerg(HDC hDC, eCommandID commandID, eIconState eState, int destX, int destY);
    void RenderCommandSlots_Zerg(HDC hDC, vector<CommandSlot>& slots, int baseX, int baseY);

    void RenderIcon_Protoss(HDC hDC, eCommandID commandID, eIconState eState, int destX, int destY);
    void RenderCommandSlots_Protoss(HDC hDC, vector<CommandSlot>& slots, int baseX, int baseY);

    const TCHAR* GetIcon_Protoss(eCommandID commandId, eIconState state);
    const TCHAR* GetIcon_Zerg(eCommandID commandID, eIconState state);

    //아이콘 RECT, Index 얻기
    RECT GetIconRect(int iconIndex, eIconState eState);
    int GetIconIndex(eCommandID commandID);
    void SetButtonFeedback(int slotIndex, bool pressed);
public:
    void UpdateUIInfoFromSelection();
private:
    void LoadProtossResource();
    void LoadZergResource();
private:
    eRaceType m_eRaceType;
private:
    static CUIMgr* m_pInstance;

    CMainUI* m_pMainUI = nullptr;
private:
    HDC m_hCachedDC = nullptr;
    HBITMAP m_hCachedBitmap = nullptr;
    bool m_bNeedRedraw = true;
    //버튼 피드백
    int m_iActiveFeedbackSlot = -1; //현재 피드백 활성 슬롯(-1일 경우 존재X)
    float m_fFeedbackTimer = 0.f;
    static constexpr float FEEDBACK_DURATION = 0.15f;
private:
    static const int ICON_WIDTH = 44;
    static const int ICON_HEIGHT = 44;
    static const int SLOT_GAP_X = 11;
    static const int SLOT_GAP_Y = 6;
};