#define NOMINMAX
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
#include "CSoundMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"
#include "CResourceMgr.h"
#include "CGameDataMgr.h"
#include "CDrone.h"

CBuilding::CBuilding() : m_bGhost(false), m_bConstructing(false), m_bComplete(false), m_bCanPlace(false),
	m_iHP(0), m_iMaxHP(10), m_iShield(0), m_iMaxShield(0), m_fConstructRemain(0.f),
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
	//공통 빌딩 애니메이션 템플릿 
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildTemplate.bmp", L"BUILD_TEMPLATE");
}

int CBuilding::Update()
{
	if (m_bDead)
		return DEAD;

	//핫키 업데이트
	UpdateHotKeys();

	switch (m_eState)
	{
	case eBuildingState::LIFT:
		Lift();
		break;
	case eBuildingState::FLYING:
		UpdateMove();
		break;
	case eBuildingState::LANDING:
		Landing();
		break;
	case eBuildingState::GHOST:
		m_bCanPlace = CanPlace({ m_tInfo.fX, m_tInfo.fY });
		break;
	case eBuildingState::CONSTRUCTING:
		UpdateConstructing();
		break;
	case eBuildingState::CONSTRUCT:
		//건설 완료 : 자식 클래스 유닛에서 유닛 생산 처리
		//UpdateProduction();
		UpdateAnimation();
		UpdateAttack();
		break;
	case eBuildingState::ATTACK:
		//UpdateAttack();
		break;
	case eBuildingState::DESTROY:
		UpdateDestroy();
		Set_Dead();
		break;
	default:
		break;
	}

	if (m_eState == eBuildingState::DESTROY)
		return DEAD;

	//빌딩 생산 정보 처리 
	UpdateBuildingUIInfo();

	__super::Update_Rect();

	return 0;
}

void CBuilding::Late_Update()
{
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

	float srcX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float srcY = CScrollMgr::Get_Instance()->Get_ScrollY();

	int row0 = m_iPlaceRow;
	int col0 = m_iPlaceCol;

	int tileDrawX = (int)(col0 * TILECX - srcX);
	int tileDrawY = (int)(row0 * TILECY - srcY);

	int buildingWidth = m_iWidth * TILECX;
	int buildingHeight = m_iHeight * TILECY;

	// 1. 고스트면: 타일 오버레이 먼저!
	if (m_bGhost)
	{
		//종족 별 타일 기반 오버레이 그리기 - 공통
		int requiredValue = GetRequiredTileValue();
		eRaceType type = m_eOriginalRace;
		CTileMgr::Get_Instance()->RenderBuildingOverlay(hdc, row0, col0, 
			m_iWidth, m_iHeight, type, m_eType ,requiredValue);

		//종족별 고스트 렌더링
		if (m_eOriginalRace == eRaceType::RACE_TERRAN)
		{
			HDC hBuildingDC = CBmpMgr::Get_Instance()->Find_Image(m_szGreenKey);

			BITMAP bmpInfo;
			HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hBuildingDC, OBJ_BITMAP);
			GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);

			int imageWidth = bmpInfo.bmWidth;
			int imageHeight = bmpInfo.bmHeight;

			int offsetX = (buildingWidth - imageWidth) * 0.5;
			int offsetY = (buildingHeight - imageHeight) * 0.5;

			int finalDrawX = tileDrawX + offsetX;
			int finalDrawY = tileDrawY + offsetY;

			//오버레이 위에 건물 이미지 그리기
			GdiTransparentBlt(hdc,
				finalDrawX, finalDrawY,
				imageWidth, imageHeight,
				hBuildingDC,
				0, 0,
				imageWidth, imageHeight,
				RGB(0, 255, 0));
		}
		else
		{
			//파일런 범위 렌더링
			RenderPylonPower(hdc);

			if (m_eType == eBuildingType::PYLON || m_eType == eBuildingType::CYBERNETICS_CORE)
			{
				ProtossBmpRender(hdc);
				return;
			}
			CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(m_szGreenKey);
			if (pPng)
			{
				int imageWidth = pPng->Get_Width();
				int imageHeight = pPng->Get_Height();
				int offsetX = (buildingWidth - imageWidth) * 0.5;
				int offsetY = (buildingHeight - imageHeight) * 0.5;

				int finalDrawX = tileDrawX + offsetX;
				int finalDrawY = tileDrawY + offsetY;
				pPng->Render_Alpha(hdc, finalDrawX, finalDrawY,
					imageWidth, imageHeight, false);
			}
		}
	}
	else
	{
		if (m_eOriginalRace == eRaceType::RACE_TERRAN)
		{
			//2. 건설 애니메이션 재생!
			HDC hBuildTemplate = CBmpMgr::Get_Instance()->Find_Image(L"BUILD_TEMPLATE");
			if (hBuildTemplate)
			{
				//3 * 3 시트에서 현재 애니메이션 프레임 계산
				int row = m_iConstructFrame / 3; //row는 몫
				int col = m_iConstructFrame % 3; //col은 나머지
				int frameWidth = 160; //템플릿 한 칸 사이즈 
				int frameHeight = 128;
				int drawX = (int)(m_tInfo.fX - frameWidth * 0.5f - srcX);
				int drawY = (int)(m_tInfo.fY - frameHeight * 0.5f - srcY);
				GdiTransparentBlt(hdc,
					drawX,       // 화면 X
					drawY,      // 화면 Y
					frameWidth,                             // 화면 너비
					frameHeight,                            // 화면 높이
					hBuildTemplate,            // 소스 DC
					col * frameWidth,                      // 소스 X (열)
					row * frameHeight,                     // 소스 Y (행)
					frameWidth,                             // 소스 너비
					frameHeight,                            // 소스 높이
					RGB(0, 255, 0));                         // Green 투명색
			}
		}
		else if (m_eOriginalRace == eRaceType::RACE_PROTOSS)
		{
			RenderProtossConstructAnim(hdc);
		}
		else if (m_eOriginalRace == eRaceType::RACE_ZERG)
		{
			RenderZergConstructAnim(hdc);
		}
	}
}

void CBuilding::Release()
{
}

void CBuilding::Destroy()
{
}

void CBuilding::RenderPylonPower(HDC hDC)
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
	m_bConstructing = true;
	m_eState = eBuildingState::CONSTRUCTING;
	m_fConstructRemain = m_fConstructDuration;
	m_iHP = 1;
	// 5. 타일 점유
	AppplyOccupy();
	// 6. 파일런의 경우 파워 설정
	if (m_eType == eBuildingType::PYLON)
	{
		ApplyPylonPower();
	}
	//저그 건물의 경우 크립 확장
	if (m_eOriginalRace == eRaceType::RACE_ZERG)
	{
		ApplyCrip();
	}
	//미리 추가하되, 개별 건물의 render에서 constructing = true이면 render하지 않기!
	CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, this);

	return true;
}

void CBuilding::UpdateConstructing()
{
	float fDT = CTimeMgr::Get_Instance()->GetDT();

	switch (m_eOriginalRace)
	{
	case eRaceType::RACE_TERRAN:
		CSoundMgr::Get_Instance()->PlaySound(L"Build/SupplyDepot.wav", SOUND_WORLD, 0.5f);
		break;
	case eRaceType::RACE_ZERG:
		break;
	case eRaceType::RACE_PROTOSS:
		//건설 시작 사운드 계속 재생
		CSoundMgr::Get_Instance()->PlaySound(L"Build/protoss-electric.mp3", SOUND_WORLD ,1.f);
		break;
	default:
		break;
	}

	m_fConstructRemain -= fDT;
	if (m_fConstructRemain < 0.f)
		m_fConstructRemain = 0.f;
	//전체 진행률 계산
	m_fProgress = 1.f - (m_fConstructRemain / m_fConstructDuration);
	if (m_fProgress > 1.f)
		m_fProgress = 1.f;
	//HP 증가 
	m_iHP = max(1, (int)(m_iMaxHP * m_fProgress));

	//빌딩 템플릿 사이즈, 160 * 128, green
	//3 * 3 애니메이션 프레임 업데이트 - fProgress - 0.1f일 경우 1로 설정
	m_iConstructFrame = (int)(m_fProgress * 10.f); //진행도에 따른 애니메이션 프레임 설정
	if (m_iConstructFrame >= 9) //전체 시트수 9개
	{
		m_iConstructFrame = 8; //8로 고정
	}
	//건설 완료 체크
	if (m_fConstructRemain <= 0.f)
	{
		m_iHP = m_iMaxHP;
		m_bComplete = true;
		m_bConstructing = false;
		m_fProgress = 0.f; //다시 건설 시간 초기화
		m_eState = eBuildingState::CONSTRUCT;
		//건설 사운드 재생 멈추고, 완료 사운드 재생
		CSoundMgr::Get_Instance()->StopSound(SOUND_WORLD);
		PlayCompleteSound();
		//건설이 완료되고 난 이후에 CObjMgr에 추가!!!!
		//CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, this);
		//건물이 배럭, 군수공장일 경우 CObjMgr의 배럭 건설 상태 true로 변경
		if (m_eType == eBuildingType::BARRACKS)
			CObjMgr::Get_Instance()->SetBarrackBuilt(true);
		if (m_eType == eBuildingType::FACTORY)
			CObjMgr::Get_Instance()->SetFactoryBuilt(true);
		if (m_eType == eBuildingType::ACADEMY)
			CObjMgr::Get_Instance()->SetAcademyBuilt(true);
		//프로토스 관문, 로보틱스, 스타게이트
		if (m_eType == eBuildingType::GATEWAY)
			CObjMgr::Get_Instance()->SetGateBuilt(true);
		if (m_eType == eBuildingType::ROBOTICS_FACILITY)
			CObjMgr::Get_Instance()->SetRoboticsBuilt(true);
		if (m_eType == eBuildingType::STARGATE)
			CObjMgr::Get_Instance()->SetStargateBuilt(true);
		//저그 - 스포닝풀, 히드라덴, 뮤탈덴, 울트라덴
		if (m_eType == eBuildingType::SPAWNING_POOL)
			CObjMgr::Get_Instance()->SetSpawningPoolBuilt(true);
		if (m_eType == eBuildingType::HYDRALISK_DEN)
			CObjMgr::Get_Instance()->SetHydraliskDenBuilt(true);
		if (m_eType == eBuildingType::SPIRE)
			CObjMgr::Get_Instance()->SetSpireBuilt(true);
		if (m_eType == eBuildingType::ULTRALISK_DEN)
			CObjMgr::Get_Instance()->SetUltraliskDenBuilt(true);
		//보급고, 파일런일 경우 인구수 증가 
		if (m_eType == eBuildingType::SUPPLY_DEPOT || m_eType == eBuildingType::PYLON)
		{
			CResourceMgr::Get_Instance()->AddDepotSupply(8);
		}
		//드론 죽이기
		if (m_eType == eBuildingType::HATCHERY || m_eType == eBuildingType::SPAWNING_POOL ||
			m_eType == eBuildingType::HYDRALISK_DEN || m_eType == eBuildingType::SPIRE ||
			m_eType == eBuildingType::ULTRALISK_DEN)
		{
			list<CObj*>& unitList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_UNIT);
			for (auto& pObj : unitList)
			{
				CDrone* pDrone = dynamic_cast<CDrone*>(pObj);
				if (pDrone)
				{
					pDrone->Set_Dead();
				}
			}
		}
	}
}

void CBuilding::RenderProtossConstructAnim(HDC hDC)
{
	int curFrame = (int)(m_fProgress * 21.f);
	if (curFrame >= 21) curFrame = 20;

	TCHAR szPath[128];

	switch (m_eType)
	{
	case eBuildingType::NEXUS:
		wsprintf(szPath, L"Nexus_Build_Template_%d", curFrame);
		break;
	case eBuildingType::PYLON:
		wsprintf(szPath, L"Pylon_Build_Template_%d", curFrame);
		break;
	case eBuildingType::ASSIMILATOR:
		wsprintf(szPath, L"Assimilator_Build_Template_%d", curFrame);
		break;
	case eBuildingType::GATEWAY:
		wsprintf(szPath, L"Gateway_Build_Template_%d", curFrame);
		break;
	case eBuildingType::FORGE:
		wsprintf(szPath, L"Forge_Build_Template_%d", curFrame);
		break;
	case eBuildingType::CYBERNETICS_CORE:
		wsprintf(szPath, L"Cybernetics_Core_Build_Template_%d", curFrame);
		break;
	case eBuildingType::SHIELD_BATTERY:
		wsprintf(szPath, L"ShieldBattery_Build_Template_%d", curFrame);
		break;
	case eBuildingType::ROBOTICS_FACILITY:
		wsprintf(szPath, L"Robotics_Build_Template_%d", curFrame);
		break;
	case eBuildingType::STARGATE:
		wsprintf(szPath, L"Stargate_Build_Template_%d", curFrame);
		break;
	case eBuildingType::CITADEL_OF_ADUN:
		wsprintf(szPath, L"CitadelOfAdun_Build_Template_%d", curFrame);
		break;
	case eBuildingType::ROBOTICS_SUPPORT_BAY:
		wsprintf(szPath, L"RoboticsSupportBay_Build_Template_%d", curFrame);
		break;
	case eBuildingType::FLEET_BEACON:
		wsprintf(szPath, L"FleetBeacon_Build_Template_%d", curFrame);
		break;
	case eBuildingType::TEMPLAR_ARCHIVES:
		wsprintf(szPath, L"TemplarArchives_Build_Template_%d", curFrame);
		break;
	case eBuildingType::OBSERVATORY:
		wsprintf(szPath, L"Observatory_Build_Template_%d", curFrame);
		break;
	case eBuildingType::ARBITER_TRIBUNAL:
		wsprintf(szPath, L"ArbiterTribunal_Build_Template_%d", curFrame);
		break;
	default:
		break;
	}
	CMyPng* pBuildTemp = CBmpMgr::Get_Instance()->Find_Png_Anim(szPath);

	if (pBuildTemp)
	{
		int iWidth = pBuildTemp->Get_Width();
		int iHeight = pBuildTemp->Get_Height();
		float srcX = CScrollMgr::Get_Instance()->Get_ScrollX();
		float srcY = CScrollMgr::Get_Instance()->Get_ScrollY();
		int drawX = (int)m_tInfo.fX - srcX - iWidth / 2;
		int drawY = (int)m_tInfo.fY - srcY - iHeight / 2;

		pBuildTemp->Render_Alpha(hDC, drawX, drawY, iWidth, iHeight, false);
	}
}

void CBuilding::RenderZergConstructAnim(HDC hDC)
{
	int curFrame = (int)(m_fProgress * 17.f);
	if (curFrame >= 17) curFrame = 16;

	TCHAR szPath[128];
	wsprintf(szPath, L"Zerg_Build_Template_%d", curFrame);

	CMyPng* pBuildTemp = CBmpMgr::Get_Instance()->Find_Png_Anim(szPath);

	if (pBuildTemp)
	{
		int iWidth = pBuildTemp->Get_Width();
		int iHeight = pBuildTemp->Get_Height();
		float srcX = CScrollMgr::Get_Instance()->Get_ScrollX();
		float srcY = CScrollMgr::Get_Instance()->Get_ScrollY();
		int drawX = (int)m_tInfo.fX - srcX - iWidth / 2;
		int drawY = (int)m_tInfo.fY - srcY - iHeight / 2;

		pBuildTemp->Render_Alpha(hDC, drawX, drawY, iWidth, iHeight, false);
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
}

void CBuilding::UpdateHotKeys()
{
	auto& selected = CSelectionMgr::Get_Instance()->GetSelected();

	if (selected.empty())
		return;
	//selected가 CBuilding이 아니면 업데이트 X
	for (auto* pObj : selected)
	{
		CBuilding* pBuilding = dynamic_cast<CBuilding*>(pObj);
		if (!pBuilding)
			return;
	}
	//첫번째 선택된 빌딩의 UI만 업데이트
	if (selected[0] != this)
		return;

	//Commandable 확인
	Commandable* command = dynamic_cast<Commandable*>(this);
	if (!command)
		return;

	//다중 건물 선택시 타입이 다르면 return
	if (selected.size() > 1)
	{
		eBuildingType firstType = static_cast<CBuilding*>(selected.front())->GetBuildingType();
		for (auto* pObj : selected)
		{
			if (static_cast<CBuilding*>(pObj)->GetBuildingType() != firstType)
				return;
		}
	}

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
			//선택된 건물들 전부 명령 실행
			for (auto* pObj : selected)
			{
				Commandable* cmd = dynamic_cast<Commandable*>(pObj);
				if (cmd)
				{
					CommandContext context{};
					cmd->ExecuteCommand(slots[i].commandID, context);
				}
			}
			break;
		}
	}
}

bool CBuilding::ExecuteCommand(eCommandID command, CommandContext& context)
{
	switch (command)
	{
	case eCommandID::RALLY:
		break;
	default:
		break;
	}
	return false;
}

void CBuilding::Rally()
{
}

void CBuilding::Lift()
{
}

void CBuilding::UpdateMove()
{
	if (m_OrderQ.empty()) //오더가 비어있으면 이동하지 않음
		return;

	Order& order = m_OrderQ.front();

	if (order.path.empty()) //경로가 없으면 정지 
		return;

	if (order.iPathIndex >= (int)order.path.size())
	{
		// 경로 완료 -> 정지
		return;
	}

	// 현재 목표가 되는 웨이포인트
	Vec2 target = order.path[order.iPathIndex];
	Vec2 current = { m_tInfo.fX, m_tInfo.fY };

	// 거리 계산
	Vec2 diff = { target.fX - current.fX, target.fY - current.fY };
	float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);

	// 도착 체크
	float fArriveEps = 6.f;

	if (dist <= fArriveEps)
	{
		order.iPathIndex++;

		// 경로 끝 -> 오더 완료
		if (order.iPathIndex >= (int)order.path.size())
		{
			// 최종 도착 -> 정지
			m_OrderQ.pop_front();
			return; //오더를 여기서 비워줘야 하나? 
		}
		// 다음 웨이포인트로?
		return;
	}
	// 브루드워 방식: 직진!
	Vec2 dir = { diff.fX / dist, diff.fY / dist };
	// 방향 갱신
	m_vDir = dir;

	// 위치 업데이트
	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_tInfo.fX += dir.fX * dt * m_fSpeed;
	m_tInfo.fY += dir.fY * dt * m_fSpeed;

	return;
}

void CBuilding::Landing()
{
}

void CBuilding::ClearOrder()
{
	//오더들의 포인터 정리
	for (auto& order : m_OrderQ)
	{
		if (order.pBuilding)
		{
			//아직 월드에 올라가기 전(이동 + 빌드 대기) 고스트만 유닛이 소유한 것으로 판단
			if (order.eType == eOrderType::MOVE_AND_BUILD && order.pBuilding->IsGhost())
			{
				delete order.pBuilding;
			}
		}
		//CONSTRUCTING은 월드에 이미 올라가 있으므로 delete하면 안 됨
		order.pBuilding = nullptr;
		order.pTarget = nullptr;
	}
	m_OrderQ.clear();
	//m_bActiveOrder = false;
	//m_eState = eUnitState::IDLE;
}

void CBuilding::SetGhost(bool bGhost)
{
	m_bGhost = bGhost;
	if (bGhost)
		m_eState = eBuildingState::GHOST;
}

void CBuilding::SetPlace(int row, int col)
{
	m_iPlaceRow = row;
	m_iPlaceCol = col;
}

bool CBuilding::CanPlace(const Vec2& worldPos)
{
	eRaceType type = m_eOriginalRace;

	bool canBuildTile = false;
	bool bNeedPower = true;

	switch (type)
	{
	case eRaceType::RACE_TERRAN:
		canBuildTile = CTileMgr::Get_Instance()->CanConstruct(m_iPlaceRow, m_iPlaceCol, m_iWidth, m_iHeight, GetRequiredTileValue(), m_eType);
		return canBuildTile;
	case eRaceType::RACE_ZERG:
		canBuildTile = CTileMgr::Get_Instance()->CanConstruct(m_iPlaceRow, m_iPlaceCol, m_iWidth, m_iHeight, GetRequiredTileValue(), m_eType);
		return canBuildTile;
	case eRaceType::RACE_PROTOSS:
		canBuildTile = CTileMgr::Get_Instance()->CanConstructProtoss(m_iPlaceRow, m_iPlaceCol, m_iWidth, m_iHeight, 
			GetRequiredTileValue(), m_eType);
		return canBuildTile;
	default:
		break;
	}
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

void CBuilding::ApplyPylonPower()
{
	int centerX = (int)(m_tInfo.fX / TILECX);
	int centerY = (int)(m_tInfo.fY / TILECY);
	CTileMgr::Get_Instance()->SetPylonPower(centerX, centerY, true);
}

void CBuilding::ApplyCrip()
{
	const int radius = 5;

	int centerX = (int)(m_tInfo.fX / TILECX);
	int centerY = (int)(m_tInfo.fY / TILECY);
	//반경 내 모든 타일에 크립 추가!
	for (int dy = -radius; dy <= radius; ++dy)
	{
		for (int dx = -radius; dx <= radius; ++dx)
		{
			if(dx * dx + dy * dy <= radius * radius)
				CTileMgr::Get_Instance()->AddCrip(centerX + dx, centerY + dy);
		}
	}
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

	int halfW = (m_iWidth - 1) / 2;
	int halfH = (m_iHeight - 1) / 2;

	outRow = row - halfH;
	outCol = col - halfW;

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
	case eCommandID::FIREBAT:
		return L"Training Firebat";
	case eCommandID::GHOST:
		return L"Training Ghost";
	case eCommandID::VULTURE:
		return L"Training Vulture";
	case eCommandID::GOLIATH:
		return L"Training Goliath";
	case eCommandID::TANK:
		return L"Training Tank";
	case eCommandID::BATTLECRUISER:
		return L"Training BattleCruiser";
		//유틸리티
	case eCommandID::ATTACK_UPGRADE:
		return L"Upgrade Attack";
	case eCommandID::ARMOR_UPGRADE:
		return L"Upgrade Armor";
	case eCommandID::VEHICLE_ATTACK_UPGRADE:
		return L"Upgrade Vehicle Attack";
	case eCommandID::VEHICLE_ARMOR_UPGRADE:
		return L"Upgrade Vehicle Armor";
	case eCommandID::SHIP_ATTACK_UPGRADE:
		return L"Upgrade Ship Attack";
	case eCommandID::SHIP_ARMOR_UPGRADE:
		return L"Upgrade Ship Armor";
	case eCommandID::NUCLEAR_MISSILE:
		return L"Nuclear Missile";
	case eCommandID::U238:
		return L"U238";
	case eCommandID::STEAMPACK:
		return L"StimPack";
	case eCommandID::RESTORATION:
		return L"Restoration";
	case eCommandID::OPTICAL_FLARE:
		return L"Optical Flare";
	case eCommandID::CADUCEUS_REACTOR:
		return L"Caduceus Reactor";
	case eCommandID::YAMATO:
		return L"Yamato";
		//프로토스
	case eCommandID::GATEWAY:
		return L"Warp Gateway";
	case eCommandID::STORM:
		return L"Storm";
	case eCommandID::RECALL:
		return L"Recall";
	case eCommandID::BLINK:
		return L"Blink";
	case eCommandID::RUSH:
		return L"Rush";
	default:
		return L"Producing";
	}
}

void CBuilding::TakeDamage(int iAttackDamage)
{
	eRaceType type = CGameDataMgr::Get_Instance()->Get_PlayerRace();
	if (type == eRaceType::RACE_PROTOSS)
	{
		//쉴드,HP 로직 -> 쉴드가 0이상인 경우 쉴드량과 damage를 비교해서 오버면 HP까지 까이도록 아니면 쉴드 흡수
		if (m_iShield > 0)
		{
			//쉴드가 데미지를 다 막을 수 있는 경우
			if (m_iShield >= iAttackDamage)
			{
				m_iShield -= iAttackDamage;
			}
			else
			{
				int remainDamage = iAttackDamage - m_iShield;
				m_iShield = 0;
				m_iHP -= remainDamage;
			}
		}
		else
		{
			m_iHP -= iAttackDamage;
		}
		if (m_iHP < 0) m_iHP = 0;
		if (m_iHP == 0)
		{
			m_eState = eBuildingState::DESTROY;
		}
	}
	else
	{
		m_iHP -= iAttackDamage;
		if (m_iHP < 0) m_iHP = 0;
		if (m_iHP == 0)
		{
			m_eState = eBuildingState::DESTROY;
		}
	}
}

void CBuilding::UpdateDestroy()
{
	//타일 점유 해제
	ReleaseOccupy();
	//사운드 재생
	//CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingDeath.wav", 1.f);
	//타겟 이펙트 생성
	CObj* pEffect = CAbstractFactory<CEffect>::Create(
		m_tInfo.fX, m_tInfo.fY);
	pEffect->Initialize();
	CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
	if (pEffectObj)
	{
		pEffectObj->Set_Effect(L"BUILDING_DESTROY_",
			11, 200, 200, eEffectType::COL_BASE, RGB(0, 0, 0), 0.05f, false,
			eEffectImageType::PNG);
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
	//저그, 테란 빌딩이 터졌을 경우 CObjMgr의 카운트 증가!
	switch (m_eType)
	{
	case eBuildingType::COMMAND_CENTER:
		CObjMgr::Get_Instance()->IncreaseDestroyCount();
		break;
	case eBuildingType::NEXUS:
		CObjMgr::Get_Instance()->IncreaseDestroyCount();
		break;
	case eBuildingType::HIVE:
		CObjMgr::Get_Instance()->IncreaseDestroyCount();
		break;
	case eBuildingType::HYDRALISK_DEN:
		CObjMgr::Get_Instance()->IncreaseDestroyCount();
		break;
	case eBuildingType::SPIRE:
		CObjMgr::Get_Instance()->IncreaseDestroyCount();
		break;
	case eBuildingType::SPAWNING_POOL:
		CObjMgr::Get_Instance()->IncreaseDestroyCount();
		break;
	case eBuildingType::ULTRALISK_DEN:
		CObjMgr::Get_Instance()->IncreaseDestroyCount();
		break;
	case eBuildingType::BARRACKS:
		CObjMgr::Get_Instance()->IncreaseDestroyCount();
		break;
	case eBuildingType::STARPORT:
		CObjMgr::Get_Instance()->IncreaseDestroyCount();
		break;
	case eBuildingType::FACTORY:
		CObjMgr::Get_Instance()->IncreaseDestroyCount();
		break;
	default:
		break;
	}
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
	info.eRaceType = m_eOriginalRace;
	info.IsProducing = !m_queue.empty();
	info.IsVisible = true;
	info.iHP = m_iHP;
	info.iMaxHP = m_iMaxHP;
	info.iShield = m_iShield;
	info.iMaxShield = m_iMaxShield;
	// 생산 중일 때만 큐 정보 추가
	if (!m_queue.empty())
	{
		info.fProgress = 1.0f - (m_queue.front().remainTime / m_queue.front().totalTime);
		info.pCurrentUnit = GetProductionName();
		info.eRaceType = m_eOriginalRace;

		//종족에 따른 각기 다른 Icon Key 얻기
		eRaceType type = m_eOriginalRace;

		for (size_t i = 0; i < m_queue.size() && i < 5; ++i)
		{
			BuildingUIInfo::QueueItem item;
			item.command = m_queue[i].command;
			if (type == eRaceType::RACE_TERRAN)
			{
				item.iIconKey = GetIconIndex(m_queue[i].command);
			}
			else if (type == eRaceType::RACE_PROTOSS)
			{
				item.wsIconName = GetIconName_Protoss(m_queue[i].command);
			}
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
	case eBuildingType::COMBAT_STATION:
		return L"Combat Station";
	case eBuildingType::NUCLEAR_SILO:
		return L"Nuclear Silo";
	case eBuildingType::BARRACKS:
		return L"Barracks";
	case eBuildingType::ENGINEERING_BAY:
		return L"Engineering Bay";
	case eBuildingType::TURRET:
		return L"Turret";
	case eBuildingType::ACADEMY:
		return L"Academy";
	case eBuildingType::BUNKER:
		return L"Bunker";
	case eBuildingType::FACTORY:
		return L"Factory";
	case eBuildingType::FACTORY_ADDON:
		return L"Factory AddOn";
	case eBuildingType::STARPORT:
		return L"Starport";
	case eBuildingType::STARPORT_ADDON:
		return L"Starport AddOn";
	case eBuildingType::SCIENCE_FACILITY:
		return L"Science Facility";
	case eBuildingType::SCIENCE_SECRET:
		return L"Science Secret";
	case eBuildingType::SCIENCE_PHYSICS:
		return L"Science Physics";
	case eBuildingType::ARMORY:
		return L"Armory";
	case eBuildingType::SUPPLY_DEPOT:
		return L"Supply Depot";
	case eBuildingType::REFINERY:
		return L"Refinery";
		//저그
	case eBuildingType::HATCHERY:
		return L"Hatchery";
	case eBuildingType::LAIR:
		return L"Lair";
	case eBuildingType::HIVE:
		return L"Hive";
	case eBuildingType::HYDRALISK_DEN:
		return L"Hydralisk Den";
	case eBuildingType::SPIRE:
		return L"Spire";
	case eBuildingType::SPAWNING_POOL:
		return L"Spawning Pool";
	case eBuildingType::ULTRALISK_DEN:
		return L"Ultralisk Den";
		//프로토스
	case eBuildingType::NEXUS:
		return L"Nexus";
	case eBuildingType::PYLON:
		return L"Pylon";
	case eBuildingType::ASSIMILATOR:
		return L"Assimilator";
	case eBuildingType::GATEWAY:
		return L"Gateway";
	case eBuildingType::FORGE:
		return L"Forge";
	case eBuildingType::PHOTON_CANNON:
		return L"Photon Cannon";
	case eBuildingType::CYBERNETICS_CORE:
		return L"Cybernetics Core";
	case eBuildingType::SHIELD_BATTERY:
		return L"Shield Battery";
	case eBuildingType::ROBOTICS_FACILITY:
		return L"Robotics Facility";
	case eBuildingType::STARGATE:
		return L"Stargate"	;
	case eBuildingType::CITADEL_OF_ADUN:
		return L"Citadel of Adun";
	case eBuildingType::ROBOTICS_SUPPORT_BAY:
		return L"Robotics Support Bay";
	case eBuildingType::FLEET_BEACON:
		return L"Fleet Beacon";
	case eBuildingType::TEMPLAR_ARCHIVES:
		return L"Templar Archives";
	case eBuildingType::OBSERVATORY:
		return L"Observatory";
	case eBuildingType::ARBITER_TRIBUNAL:
		return L"Arbiter Tribunal";
	default:
		return L"Building";
	}
}

const TCHAR* CBuilding::GetIconName_Protoss(eCommandID command)
{
	switch (command)
	{
	case eCommandID::PROBE:
		return L"Probe_0";
	case eCommandID::ZEALOT:
		return L"Zealot_0";
	case eCommandID::DRAGOON:
		return L"Dragoon_0";
	case eCommandID::HIGH_TEMPLAR:
		return L"HighTemplar_0";
	case eCommandID::DARK_TEMPLAR:
		return L"DarkTemplar_0";
	case eCommandID::OBSERVER:
		return L"Observer_0";
	case eCommandID::SHUTTLE:
		return L"Shuttle_0";
	case eCommandID::REAVER:
		return L"Reaver_0";
	case eCommandID::CORSAIR:
		return L"Corsair_0";
	case eCommandID::SCOUT:
		return L"Scout_0";
	case eCommandID::CARRIER:
		return L"Carrier_0";
	case eCommandID::ARBITER:
		return L"Arbiter_0";
	case eCommandID::NEXUS:
		break;
	case eCommandID::PYLON:
		break;
	case eCommandID::ASSIMILATOR:
		break;
	case eCommandID::GATEWAY:
		return L"Gateway";
	case eCommandID::FORGE:
		break;
	case eCommandID::PHOTON_CANNON:
		break;
	case eCommandID::CYBERNETICS_CORE:
		return L"Cybernetics_Core";
	case eCommandID::SHIELD_BATTERY:
		break;
	case eCommandID::ROBOTICS_FACILITY:
		break;
	case eCommandID::STARGATE:
		break;
	case eCommandID::CITADEL_OF_ADUN:
		break;
	case eCommandID::ROBOTICS_SUPPORT_BAY:
		break;
	case eCommandID::FLEET_BEACON:
		break;
	case eCommandID::TEMPLAR_ARCHIVES:
		break;
	case eCommandID::OBSERVATORY:
		break;
	case eCommandID::ARBITER_TRIBUNAL:
		break;
	case eCommandID::RUSH:
		return L"Rush_0";
	case eCommandID::BLINK:	
		return L"Blink_0";
	case eCommandID::MIND_CONTROL:
		break;
	case eCommandID::FEEDBACK:
		break;
	case eCommandID::MAELSTROM:
		break;
	case eCommandID::STORM:
		return L"Storm_0";
		break;
	case eCommandID::WRAP:
		break;
	case eCommandID::HALLUCINATION:
		return L"Hallucination_0";
		break;
	case eCommandID::INTERCEPTOR:
		break;
	case eCommandID::RECALL:
		return L"Recall_0";
		break;
	case eCommandID::ICE:
		return L"Ice_0";
		break;
	case eCommandID::NONE:
		break;
	default:
		break;
	}

	return nullptr;
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
	case eCommandID::FIREBAT:
		return IconIndex::FIREBAT;
	case eCommandID::GHOST:
		return IconIndex::GHOST;
	case eCommandID::VULTURE:
		return IconIndex::VULTURE;
	case eCommandID::GOLIATH:
		return IconIndex::GOLIATH;
	case eCommandID::TANK:
		return IconIndex::TANK;
	case eCommandID::BATTLECRUISER:
		return IconIndex::BATTLECRUISER;
		//유틸리티
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
	case eCommandID::NUCLEAR_MISSILE:
		return IconIndex::NUCLEAR_MISSILE;
	case eCommandID::YAMATO:
		return IconIndex::YAMATO;
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
	default:
		return -1;
	}
	return 0;
}


void CBuilding::UpdateAttack()
{
}

void CBuilding::FireBullet()
{
}

CObj* CBuilding::FindNearestEnemy()
{
	return nullptr;
}

int CBuilding::DirTo16WayIndex(Vec2& vDir)
{
	// (중요) 화면 좌표계가 y 아래로 증가면, 수학 좌표계로 맞추려면 -y를 쓰는 게 안전
	float ang = atan2(-vDir.fY, vDir.fX); // -PI ~ PI

	float step = PI / 8.f;        // 22.5도
	int idx = (int)floorf((ang + (PI / 16.f)) / step); // 가까운 방향 반올림
	idx = (idx % 16 + 16) % 16;   // 0~15
	return idx;
}

int CBuilding::DirTo32WayIndex(Vec2& vDir)
{
	float angle = atan2f(vDir.fY, vDir.fX);
	if (angle < 0.f)
		angle += 2.f * PI;

	int index = (int)(angle / (2.f * PI / 32.f) + 0.5f);
	return index % 32;
}

void CBuilding::ProtossBmpRender(HDC hdc)
{
	HDC hBuildingDC = CBmpMgr::Get_Instance()->Find_Image(m_szGreenKey);

	BITMAP bmpInfo;
	HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hBuildingDC, OBJ_BITMAP);
	GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);

	float srcX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float srcY = CScrollMgr::Get_Instance()->Get_ScrollY();

	int row0 = m_iPlaceRow;
	int col0 = m_iPlaceCol;

	int tileDrawX = (int)(col0 * TILECX - srcX);
	int tileDrawY = (int)(row0 * TILECY - srcY);

	int buildingWidth = m_iWidth * TILECX;
	int buildingHeight = m_iHeight * TILECY;

	int imageWidth = bmpInfo.bmWidth;
	int imageHeight = bmpInfo.bmHeight;

	int offsetX = (buildingWidth - imageWidth) * 0.5;
	int offsetY = (buildingHeight - imageHeight) * 0.5;

	int finalDrawX = tileDrawX + offsetX;
	int finalDrawY = tileDrawY + offsetY;

	//오버레이 위에 건물 이미지 그리기
	GdiTransparentBlt(hdc,
		finalDrawX, finalDrawY,
		imageWidth, imageHeight,
		hBuildingDC,
		0, 0,
		imageWidth, imageHeight,
		RGB(255, 255, 255));
}

void CBuilding::UpdateAnimation()
{
}

void CBuilding::BuildAddOn()
{
}

void CBuilding::PlayCompleteSound()
{
}

void CBuilding::RemoveOrdersWithTarget(CObj* pTarget)
{
	//터렛, 벙커, 포토캐논용 함수 - 타겟 포인터와의 연결 끊기
	if (m_pTarget == pTarget)
	{
		m_pTarget = nullptr;
	}
}