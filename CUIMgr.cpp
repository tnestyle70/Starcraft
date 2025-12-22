#include "pch.h"
#include "CUIMgr.h"
#include "CBmpMgr.h"
#include "CMainUI.h"
#include "Commandable.h"
#include "CSelectionMgr.h"
#include "CTimeMgr.h"

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
    // MainUI.bmp를 한 번만 로드
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MainUI/MainUI.bmp", L"MainUI");
    //UIButton 로드
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Icon/ButtonIcon.bmp", L"ICON_BUTTON_UI");
    // CMainUI 생성 및 초기화 (여기서 알파 비트맵 생성됨)
    m_pMainUI = new CMainUI;
    m_pMainUI->Initialize();
}

int CUIMgr::Update()
{
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
    return 0;
}

void CUIMgr::Release()
{
    if (m_pMainUI)
    {
        m_pMainUI->Release();
        delete m_pMainUI;
        m_pMainUI = nullptr;
    }
}

void CUIMgr::Render(HDC hDC)
{
    //1. 배경 렌더링
    if (m_pMainUI)
        m_pMainUI->Render(hDC);
    //선택된 오브젝트 CommandSlot 렌더링
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
    
    if (selected.size() == 1)
    {
        CObj* pObj = selected[0];
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
            RenderCommandSlots(hDC, slots, baseX, baseY);
        }
    }
}

void CUIMgr::RenderIcon(HDC hDC, eIconState eState, int iconIndex, int destX, int destY)
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

void CUIMgr::RenderCommandSlots(HDC hDC, vector<CommandSlot>& slots, int baseX, int baseY)
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
        else if (!slots[i].clickable)
            state = eIconState::GRAY; //클릭 불가능 - 회색
        //자원 부족, 다른 조건 체크 추가
        int col = i % 3;
        int row = i / 3;

        int x = baseX + col * (ICON_WIDTH + SLOT_GAP_X);
        int y = baseY + row * (ICON_HEIGHT + SLOT_GAP_Y);
        //아이콘 렌더링
        RenderIcon(hDC, state, iconIndex, x, y);
        //TODO: 단축키 텍스트 렌더링하기
    }
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

        // 유닛 생산
    case eCommandID::SCV:
        return IconIndex::SCV;
        // 건물 건설
    case eCommandID::COMMAND_CENTER:
        return IconIndex::COMMAND_CENTER;
    case eCommandID::SUPPLY_DEPOT:
        return IconIndex::SUPPLY_DEPOT;
    case eCommandID::BARRACKS:
        return IconIndex::BARRACKS;
        /*
    case eCommandID::MARIN:
        return IconIndex::MARIN;
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


