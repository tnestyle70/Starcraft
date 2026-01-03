#include "pch.h"
#include "CBuilding.h"
#include "CTimeMgr.h"
#include "CTileMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CSelectionMgr.h"
#include "CUIMgr.h"
#include "CBmpMgr.h"
#include "CObjMgr.h"
#include "CMainUI.h"
#include "CFogMgr.h"

CBuilding::CBuilding() : m_bGhost(false), m_bComplete(false), m_bCanPlace(false),
	m_iHP(0), m_iMaxHP(10), m_fConstructDuration(0.f), m_fConstructRemain(0.f),
	m_iWidth(0), m_iHeight(0), m_iSightRange(14)

{
	ZeroMemory(&m_eType, sizeof(eBuildingType));
	ZeroMemory(&m_eState, sizeof(eBuildingState));
	ZeroMemory(&m_tCost, sizeof(ResourceCost));
}

CBuilding::~CBuilding()
{
}

void CBuilding::Initialize()
{
	//CBuilding을 초기화할 때 순수 가상함수로 선언되어있는 SetBuildingData를 호출해서
	//건물 정보 초기화 일관되게 진행
	SetBuildingData();
	m_iHP = 0;
	m_eState = eBuildingState::GHOST;
}

int CBuilding::Update()
{
	if (m_eState == eBuildingState::DESTROY)
		return DEAD;
	//핫키 업데이트
	UpdateHotKeys();

	switch (m_eState)
	{
	case eBuildingState::GHOST:
		m_bCanPlace = CanPlace({ m_tInfo.fX, m_tInfo.fY });
		break;
	case eBuildingState::CONSTRUCTING:
		UpdateConstructing();
		break;
	case eBuildingState::CONSTRUCT:
		//건설 완료 : 자식 클래스 유닛에서 유닛 생산 처리
		//UpdateProduction();
		break;
	case eBuildingState::DESTROY:
		break;
	default:
		break;
	}

	//빌딩 생산 정보 처리 
	UpdateBuildingUIInfo();

	__super::Update_Rect();

	return 0;
}

void CBuilding::Late_Update()
{
}

static void DrawBuildOverlaySolid(HDC hdc, int x, int y, int w, int h, bool canPlace)
{
	COLORREF fill = canPlace ? RGB(0, 255, 0) : RGB(255, 0, 0);

	HBRUSH hBrush = CreateSolidBrush(fill);
	RECT rc{ x, y, x + w, y + h };

	// 단색 채움
	FillRect(hdc, &rc, hBrush); // :contentReference[oaicite:3]{index=3}

	// 1px 테두리
	FrameRect(hdc, &rc, hBrush); // :contentReference[oaicite:4]{index=4}

	DeleteObject(hBrush);
}

void CBuilding::Render(HDC hdc)
{
	// (기존 fog 체크 유지)
	Vec2 pos = Get_Pos();
	int fogRow, fogCol;
	if (CTileMgr::Get_Instance()->WorldToCell(pos, fogRow, fogCol))
	{
		eFogState fogState = CFogMgr::Get_Instance()->GetFogState(fogRow, fogCol);
		if (fogState == eFogState::UNKNOWN) return;
	}

	float scrX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float scrY = CScrollMgr::Get_Instance()->Get_ScrollY();

	int row0 = m_iPlaceRow;
	int col0 = m_iPlaceCol;

	int tileDrawX = (int)(col0 * TILECX - scrX);
	int tileDrawY = (int)(row0 * TILECY - scrY);

	int buildingWidth = m_iWidth * TILECX;
	int buildingHeight = m_iHeight * TILECY;

	// 2) 건물 스프라이트는 "그냥 투명키로" 렌더
	const TCHAR* pImageKey = m_szGreenKey; // 이제 빨/초 고스트 이미지 쓸 필요 없음(원본 키로 통일)
	HDC hBuildingDC = CBmpMgr::Get_Instance()->Find_Image(pImageKey);

	BITMAP bmpInfo;
	HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hBuildingDC, OBJ_BITMAP);
	GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);

	int imageWidth = bmpInfo.bmWidth;
	int imageHeight = bmpInfo.bmHeight;

	int offsetX = (buildingWidth - imageWidth) * 0.5;
	int offsetY = (buildingHeight - imageHeight) * 0.5;

	int finalDrawX = tileDrawX + offsetX;
	int finalDrawY = tileDrawY + offsetY;

	// 1) 고스트면: 타일 오버레이 먼저!
	if (m_bGhost)
	{
		//타일 기반 오버레이 그리기
		int requiredValue = GetRequiredTileValue();
		CTileMgr::Get_Instance()->RenderBuildingOverlay(hdc, row0, col0, m_iWidth, m_iHeight, requiredValue);
		//오버레이 위에 건물 이미지 그리기
		GdiTransparentBlt(hdc,
			finalDrawX, finalDrawY,
			imageWidth, imageHeight,
			hBuildingDC,
			0, 0,
			imageWidth, imageHeight,
			RGB(0, 255, 0));
	}

	// TransparentBlt 동일 계열: 색상키(여기선 초록 배경)로 투명 처리 :contentReference[oaicite:4]{index=4}
	GdiTransparentBlt(hdc,
		finalDrawX, finalDrawY,
		imageWidth, imageHeight,
		hBuildingDC,
		0, 0,
		imageWidth, imageHeight,
		RGB(0, 255, 0));
}

void CBuilding::Release()
{
}

bool CBuilding::StartConstruct(const Vec2& worldPos)
{
	//배치 가능 여부 확인
	if (!CanPlace(worldPos))
		return false;

	//최종 위치 조정
	int row, col;
	if (CalcSizeTopLeft(worldPos, row, col))
	{
		Vec2 centerPos = CTileMgr::Get_Instance()->CellToWorldCenter(
			m_iPlaceRow + GetHeight() * 0.5f,
			m_iPlaceCol + GetWidth() * 0.5f);
		Set_Pos(centerPos.fX, centerPos.fY);
		SetPlace(row, col);
	}
	//3. 리소스 차감
	if (!CResourceMgr::Get_Instance()->TrySpend(m_tCost, false))
		return false;
	// 4. 건설 시작
	m_bGhost = false;
	m_eState = eBuildingState::CONSTRUCTING;
	m_fConstructRemain = m_fConstructDuration;
	m_iHP = 1;
	// 5. 타일 점유
	AppplyOccupy();

	return true;
}

void CBuilding::UpdateConstructing()
{
	float fDT = CTimeMgr::Get_Instance()->GetDT();

	m_fConstructRemain -= fDT;
	if (m_fConstructRemain < 0.f)
		m_fConstructRemain = 0.f;
	//전체 진행률 계산
	float progress = 1.f - (m_fConstructRemain / m_fConstructDuration);
	if (progress > 1.f)
		progress = 1.f;

	m_iHP = max(1, (int)(m_iMaxHP * progress));

	if (m_fConstructRemain <= 0.f)
	{
		m_iHP = m_iMaxHP;
		m_bComplete = true;
		m_eState = eBuildingState::CONSTRUCT;
	}
}

void CBuilding::CommandCardSlot(vector<CommandSlot>& outSlot)
{
	outSlot.clear();
	outSlot.resize(9);
	//미리 값 채우기
	for (int i = 0; i < 9; ++i)
	{
		outSlot[i].slotIndex = i;
		outSlot[i].commandID = eCommandID::NONE;
		outSlot[i].iconKey = TEXT("");
		outSlot[i].hotkey = 0;
		outSlot[i].clickable = false;
		outSlot[i].visible = false;
	}
	//건설 중에는 커맨드 카드 표시 X
	if (m_eState == eBuildingState::CONSTRUCTING)
		return;
	//3 * 3 기준
	//6번 슬롯
	if (m_eState == eBuildingState::CONSTRUCT)
	{
		outSlot[5].commandID = eCommandID::RALLY;
		outSlot[5].iconKey = TEXT("ICON_RALLY");
		outSlot[5].hotkey = 'R';
		outSlot[5].clickable = true;
		outSlot[5].visible = true;
	}
}

void CBuilding::UpdateHotKeys()
{
	auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
	//선택된 유닛이 없거나 CUnit 클래스가 아닐 경우 return
	if (selected.size() != 1 || selected[0] != this)
		return;
	//Commandable 확인
	Commandable* command = dynamic_cast<Commandable*>(this);
	if (!command)
		return;
	//슬롯 정보
	vector<CommandSlot> slots;
	command->CommandCardSlot(slots);

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
			command->ExecuteCommand(slots[i].commandID, context);
			break;
		}
	}
}

bool CBuilding::ExecuteCommand(eCommandID command, CommandContext& context)
{
	switch (command)
	{
	case eCommandID::RALLY:
		//랠리 포인트 설정
		break;
	default:
		break;
	}
	return false;
}

void CBuilding::SetGhost(bool bGhost)
{
	m_bGhost = bGhost;
	if (bGhost)
		m_eState = eBuildingState::GHOST;
}

void CBuilding::SetPlace(int row, int col)
{
	wchar_t buf[128];
	swprintf_s(buf, L"SetPlace: row=%d col=%d", row, col);
	OutputDebugString(buf);

	m_iPlaceRow = row;
	m_iPlaceCol = col;
}

bool CBuilding::CanPlace(const Vec2& worldPos)
{
	//타일 점유 빌드 가능 영역 검사 
	CTileMgr* tileMgr = CTileMgr::Get_Instance();

	wchar_t buf[256];
	swprintf_s(buf, L"CanPlace: using m_iPlaceRow=%d m_iPlaceCol=%d",
		m_iPlaceRow, m_iPlaceCol);
	OutputDebugString(buf);

	//int row, col;
	//if (!CalcSizeTopLeft(worldPos, row, col))
	//	return false;
	int requiredValue = GetRequiredTileValue();
	return tileMgr->CanConstruct(m_iPlaceRow, m_iPlaceCol, m_iWidth, m_iHeight, requiredValue);
}

void CBuilding::AppplyOccupy()
{
	//배치하고 난 이후 점유 처리
	CTileMgr* tileMgr = CTileMgr::Get_Instance();
	int row0, col0;
	//배치 확정할 때 row, col 저장 
	if (!CalcSizeTopLeft(Get_Pos(), row0, col0)) return;

	for (int r = row0; r < row0 + m_iHeight; ++r)
		for (int c = col0; c < col0 + m_iWidth; ++c)
			tileMgr->SetOccupy(r, c, true);
}

void CBuilding::ReleaseOccupy()
{
	CTileMgr* tileMgr = CTileMgr::Get_Instance();
	int row0, col0;
	//배치 확정할 때 row, col 저장 
	if (!CalcSizeTopLeft(Get_Pos(), row0, col0)) return;

	for (int r = row0; r < row0 + m_iHeight; ++r)
		for (int c = col0; c < col0 + m_iWidth; ++c)
			tileMgr->SetOccupy(r, c, false);
}

bool CBuilding::CalcSizeTopLeft(const Vec2& worldPos, int& outRow, int& outCol) const
{
	CTileMgr* tileMgr = CTileMgr::Get_Instance();

	int row, col;
	if (!tileMgr->WorldToCell(worldPos, row, col)) return false;

	wchar_t buf[256];
	swprintf_s(buf, L"CalcSize: worldPos=(%.1f,%.1f) → cell=(%d,%d) width=%d height=%d",
		worldPos.fX, worldPos.fY, row, col, m_iWidth, m_iHeight);
	OutputDebugString(buf);

	int halfW = (m_iWidth - 1) / 2;
	int halfH = (m_iHeight - 1) / 2;

	outRow = row - halfH;
	outCol = col - halfW;

	swprintf_s(buf, L"  → topLeft=(%d,%d) halfW=%d halfH=%d",
		outRow, outCol, halfW, halfH);
	OutputDebugString(buf);

	return true;
}

RECT CBuilding::GetWorldRect() const
{
	RECT rc{};

	rc.left = (LONG)(m_tInfo.fX - m_tInfo.fCX * 0.5f);
	rc.top = (LONG)(m_tInfo.fY - m_tInfo.fCY * 0.5f);
	rc.right = (LONG)(m_tInfo.fX + m_tInfo.fCX * 0.5f);
	rc.bottom = (LONG)(m_tInfo.fY + m_tInfo.fCY * 0.5f);

	return rc;
}

const TCHAR* CBuilding::GetProductionName()
{
	if (m_queue.empty())
		return L"";

	switch (m_queue.front().command)
	{
	case eCommandID::SCV:
		return L"Training SCV";
	case eCommandID::MARINE:
		return L"Training Marine";
	case eCommandID::MEDIC:
		return L"Training Medic";
	case eCommandID::VULTURE:
		return L"Training Vulture";
	case eCommandID::TANK:
		return L"Training Tank";
	default:
		return L"Producing";
	}
}

void CBuilding::TakeDamage(int iAttackDamage)
{
	if (IsGhost() || !IsComplete())
		return;
	m_iHP -= iAttackDamage;
	if (m_iHP < 0) m_iHP = 0;
	if (m_iHP == 0) Set_Dead();
}

void CBuilding::UpdateBuildingUIInfo()
{
	auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
	//해당 건물이 선택되어있는 상태인지 확인
	bool bThisSelected = (selected.size() == 1 && selected[0] == this);
	if (!bThisSelected)
		return;
	//선택되지 않은 상태면 넘어감
	if (!bThisSelected)
	{
		//선택이 해제 되었을 경우 UI 숨기기
		BuildingUIInfo info;
		info.IsVisible = false;
		info.pBuildingName = nullptr;
		info.pCurrentUnit = nullptr;
		info.IsProducing = false;
		info.fProgress = 0.f;
		info.queue.clear();
		info.iHP = 0;
		info.iMaxHP = 0;
		CMainUI::Get_Instance()->SetBuildingUIInfo(info);
		return;
	}
	//선택된 상태라면 항상 건물 정보 표시
	BuildingUIInfo info;
	info.pBuildingName = GetBuildingName();
	info.eType = GetBuildingType();
	info.IsProducing = !m_queue.empty();
	info.IsVisible = true;
	info.iHP = m_iHP;
	info.iMaxHP = m_iMaxHP;
	// 생산 중일 때만 큐 정보 추가
	if (!m_queue.empty())
	{
		info.fProgress = 1.0f - (m_queue.front().remainTime / m_queue.front().totalTime);
		info.pCurrentUnit = GetProductionName();

		for (size_t i = 0; i < m_queue.size() && i < 5; ++i)
		{
			BuildingUIInfo::QueueItem item;
			item.command = m_queue[i].command;
			item.iIconKey = GetIconIndex(m_queue[i].command);
			info.queue.push_back(item);
		}
	}
	else
	{
		info.pCurrentUnit = nullptr;
		info.fProgress = 0.f;
	}

	CMainUI::Get_Instance()->SetBuildingUIInfo(info);
}

const TCHAR* CBuilding::GetBuildingName()
{
	switch (m_eType)
	{
	case eBuildingType::COMMAND_CENTER:
		return L"Command Center";
	case eBuildingType::BARRACKS:
		return L"Barracks";
	case eBuildingType::FACTORY:
		return L"Factory";
	case eBuildingType::STARPORT:
		return L"Starport";
	case eBuildingType::SUPPLY_DEPOT:
		return L"Supply Depot";
	case eBuildingType::REFINERY:
		return L"Refinery";
	default:
		return L"Building";
	}
}

int CBuilding::GetIconIndex(eCommandID command)
{
	switch (command)
	{
	case eCommandID::SCV:
		return IconIndex::SCV;
	case eCommandID::MARINE:
		return IconIndex::MARINE;
	case eCommandID::MEDIC:
		return IconIndex::MEDIC;
	case eCommandID::VULTURE:
		return IconIndex::VULTURE;
	case eCommandID::TANK:
		return IconIndex::TANK;
	case eCommandID::BATTLECRUISER:
		return IconIndex::BATTLECRUISER;
		break;
	default:
		return -1;
	}

	return 0;
}