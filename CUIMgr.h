#pragma once
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
    const int FACTORY = 30; //군수공장
    const int STARPORT = 31; //스타포트
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
    //단일 아이콘 렌더
    void RenderIcon(HDC hDC, eIconState eState, int iconIndex,
        int destX, int destY);
    //커맨드 슬롯 9개 렌더링(배치 렌더링)
    void RenderCommandSlots(HDC hDC, vector<CommandSlot>& slots, int baseX, int baseY);
    //아이콘 RECT, Index 얻기
    RECT GetIconRect(int iconIndex, eIconState eState);
    int GetIconIndex(eCommandID commandID);
    void SetButtonFeedback(int slotIndex, bool pressed);
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