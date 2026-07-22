#include "pch.h"
#include "CStage.h"
#include "CBmpMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CScrollMgr.h"
#include "CTimeMgr.h"
#include "CInputMgr.h"
#include "CSceneMgr.h"
#include "CPortraitMgr.h"
#include "CSelectionMgr.h"
#include "CTileMgr.h"
#include "CCommandMgr.h"
#include "CNavMgr.h"
#include "CMarine.h"
#include "CBattleCruiser.h"
#include "CTank.h"
#include "CSCV.h"
#include "CMedic.h"
#include "CVulture.h"
#include "CUIMgr.h"
#include "CResourceMgr.h"
#include "CCommandMgr.h"
#include "CMainUI.h"
#include "CSteeringMgr.h"
#include "CCollisionMgr.h"
#include "CFogMgr.h"
#include "CGoliath.h"
#include "CCursorMgr.h"
#include "CMineral.h"
#include "CGas.h"
#include "CSoundMgr.h"
#include "CFirebat.h"
#include "CGhost.h"
#include "CBlueMarine.h"
#include "CZergling.h"
#include "CHydralisk.h"
#include "CUltralisk.h"
#include "CAnimButton.h"
#include "CZealot.h"
#include "CMutalisk.h"
#include "COverload.h"
#include "CHive.h"
#include "CHydraliskDen.h"
#include "CSpire.h"
#include "CSpawningPool.h"
#include "CUltraliskDen.h"
#include "CButton.h"
#include "CMainUI.h"
#include "CNexus.h"
#include "CProbe.h"
#include "CDrone.h"
#include "CGameDataMgr.h"
#include "CShuttle.h"
#include "CArchon.h"
#include "CDarkArchon.h"
#include "CDarkTemplar.h"
#include "CDragon.h"
#include "CArbiter.h"
#include "CCarrier.h"
#include "CCorsair.h"
#include "CHighTemplar.h"
#include "CReavor.h"
#include "CScout.h"
#include "CObserver.h"
#include "CInterceptor.h"
#include "CLava.h"

CStage::CStage()
{
}

CStage::~CStage()
{
}

void CStage::Initialize()
{
	CTimeMgr::Get_Instance()->Initialize();

	//리소스 로드
	LoadTerranResource();
	LoadProtossResource();
	LoadZergResource();

	eRaceType eRaceType = CGameDataMgr::Get_Instance()->Get_PlayerRace();
	switch (eRaceType)
	{
	case eRaceType::RACE_TERRAN:
		//BGM사운드 재생
		CSoundMgr::Get_Instance()->PlayBGM(L"BGM/TerranBGM1.wav", 1.f);
		SpawnTerranBase();
		break;
	case eRaceType::RACE_ZERG:
		SpawnZergBase();
		break;
	case eRaceType::RACE_PROTOSS:
		//BGM사운드 재생
		CSoundMgr::Get_Instance()->PlayBGM(L"etc/protoss.wav", 1.f);
		SpawnProtossBase();
		break;
	default:
		break;
	}
	/*
   	LoadTerranResource();
	LoadProtossResource();
	LoadZergResource();
	//유닛, 건물 생성
	SpawnTerranBase();
	SpawnProtossBase();
	SpawnZergBase();
	CreateObject();
	CreateZergEnemy();
	CreateZergBuildings();
	//CreateTerrranEnemyTest();
	//CreateZergEnemyTest();
	*/
	//맵 데이터
	CTileMgr::Get_Instance()->Load_Tile();
	CTileMgr::Get_Instance()->Initialize(); //크립만 초기화
	//미네랄, 가스 자원 로딩!!
	CreateResource();
	//NavGrid 정보 로드
	CNavMgr::Get_Instance()->BuildFromTile();
	//전장의 안개
	CFogMgr::Get_Instance()->Initialize();
	//UIMgr 초기화 (여기서 MainUI.bmp 로드 + 알파 비트맵 생성)
	CUIMgr::Get_Instance()->Initialize();
	//CursorMgr 초기화
	CCursorMgr::Get_Instance()->Initialize();
	//유닛이 생성된 이후에 FogUpdate
	CFogMgr::Get_Instance()->UpdateVision();
	//리소스 매니저 - 나중에 지우기
	CResourceMgr::Get_Instance()->Reset();
	//portrait 매니저 
	CPortraitMgr::Get_Instance()->Initialize();
	//Stage 진입시 마우스 클램핑
	ClampMouse(g_hWnd);
}

int CStage::Update()
{
	//CTimeMgr::Get_Instance()->BeginFrame();

	CSelectionMgr::Get_Instance()->Update();

	CObjMgr::Get_Instance()->Update();
	//충돌 처리만 진행
	list<CObj*>* pUnitList = CObjMgr::Get_Instance()->Get_ObjListPointer(OBJ_UNIT);
	if (pUnitList && !pUnitList->empty())
	{
		list<CObj*> allUnits = *pUnitList;
		// 1. 유닛끼리 밀어내기 
		CCollisionMgr::PushApart_Units(allUnits, 10.f);
		// 2. 벽 충돌 보정
		for (auto* pObj : allUnits)
		{
			CCollisionMgr::ResolveWallCollision(pObj);
		}
	}
	CFogMgr::Get_Instance()->Update();

	CMainUI::Get_Instance()->UpdateMinimapFog();

	CUIMgr::Get_Instance()->Update();

	CPortraitMgr::Get_Instance()->Update();

	CCommandMgr::Get_Instance()->Update();

	CCursorMgr::Get_Instance()->Update();

	return 0;
}

void CStage::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();

	ClampMouse(g_hWnd);

	float fDT = CTimeMgr::Get_Instance()->GetDT();

	CScrollMgr::Get_Instance()->Update_EdgeScroll(fDT);

	if (CInputMgr::Get_Instance()->KeyDown(LEFT_MOUSE))
	{
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(g_hWnd, &mousePos); //화면 좌표 -> 클라이언트 좌표
		//스크롤 보정하지 않은 값 전달
		CMainUI::Get_Instance()->HandleMinimapClick(mousePos);
	}

	//화면 클램핑 풀기
	if (CInputMgr::Get_Instance()->KeyPressVK(VK_F4))
	{
		ClipCursor(nullptr);
	}

	//강제 종료
	if (CInputMgr::Get_Instance()->KeyPress(ESCAPE0))
	{
		Release();
		ClipCursor(nullptr);
		//CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_MENU);
		DestroyWindow(g_hWnd);
	}
	//스테이지로 이동
	if (CInputMgr::Get_Instance()->KeyDown(P_KEY))
	{
		CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_MENU);
	}

	StartRush(); //저그 러쉬 시작!

	CheckEndGame(); //게임 종료 체크 
}

void CStage::Render(HDC hDC)
{
	// 1. 배경 그리기 (스크롤 적용)
	HDC hStageDC = CBmpMgr::Get_Instance()->Find_Image(L"Stage");
	if (hStageDC)
	{
		int scrX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
		int scrY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

		// Clamp 및 BitBlt (작성하신 코드 유지)
		BitBlt(hDC, 0, 0, WINCX, WINCY, hStageDC, scrX, scrY, SRCCOPY);
	}
	//파일런 범위 렌더링
	Render_Pylon_Power(hDC);
	Render_Shuttle_Power(hDC);
	//저그 - 크립 렌더링
	CTileMgr::Get_Instance()->RenderCrip(hDC);
	// 초록 선택 원 표시
	CSelectionMgr::Get_Instance()->RenderSelectionCircle(hDC);
	// 오브젝트 렌더
	CObjMgr::Get_Instance()->Render(hDC);
	// 커맨드 매니저
	CCommandMgr::Get_Instance()->Render(hDC);
	// 전장의 안개
	CFogMgr::Get_Instance()->Render(hDC);
	// UI 렌더 (가장 위)
	CUIMgr::Get_Instance()->Render(hDC);

	CSelectionMgr::Get_Instance()->Render(hDC);
	// 커서 
	CCursorMgr::Get_Instance()->Render(hDC);

	//모든 프레임 업데이트가 종료된 이후에 지워버리기
	CObjMgr::Get_Instance()->CleanUpDeadObject();
}

void CStage::Release()
{
	CObjMgr::Get_Instance()->Delete_Obj(OBJ_UNIT);
	CObjMgr::Get_Instance()->Delete_Obj(OBJ_BUILDING);
	CObjMgr::Get_Instance()->Delete_Obj(OBJ_RESOURCE);
	CObjMgr::Get_Instance()->Delete_Obj(OBJ_PROJECTILE);
	CObjMgr::Get_Instance()->Delete_Obj(OBJ_ENEMY);
	CObjMgr::Get_Instance()->Delete_Obj(OBJ_UI);
	CUIMgr::Get_Instance()->Release();
	CFogMgr::Get_Instance()->Release();
	CSelectionMgr::Get_Instance()->Release();
	//BGM 종료하기!
	CSoundMgr::Get_Instance()->StopSound(SOUND_BGM);
}

void CStage::Render_Pylon_Power(HDC hDC)
{
	eRaceType type = CGameDataMgr::Get_Instance()->Get_PlayerRace();
	if (type != eRaceType::RACE_PROTOSS)
		return;
	vector<CBuilding*> buildingList = CObjMgr::Get_Instance()->GetBuildings();
	for (auto& pBuilding : buildingList)
	{
		if (pBuilding->GetBuildingType() == eBuildingType::PYLON)
		{
			CPylon* pPylon = dynamic_cast<CPylon*>(pBuilding);
			if (pPylon)
			{
				pPylon->RenderPylonPower(hDC);
			}
		}
	}
}

void CStage::Render_Shuttle_Power(HDC hDC)
{
	eRaceType type = CGameDataMgr::Get_Instance()->Get_PlayerRace();
	if (type != eRaceType::RACE_PROTOSS)
		return;
	vector<CUnit*> unitList = CObjMgr::Get_Instance()->GetUnits();
	for (auto& pUnit : unitList)
	{
		if (pUnit->Get_UnitType() == eUnitType::SHUTTLE)
		{
			CShuttle* pShuttle = dynamic_cast<CShuttle*>(pUnit);
			if (pShuttle)
			{
				pShuttle->RenderPower(hDC);
			}
		}
	}
}

void CStage::StartRush()
{
	eRaceType type = CGameDataMgr::Get_Instance()->Get_PlayerRace();

	//디펜스 형식으로 F1, F2, F3에 따른 각기 다른 웨이브를 생성
	if (CInputMgr::Get_Instance()->KeyDownVK(VK_F1))
	{
		CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingBirth.wav", 0.5f);
		CObjMgr::Get_Instance()->SetZergRush(true);
	}
	if (CInputMgr::Get_Instance()->KeyDownVK(VK_F2))
	{
		CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingBirth.wav", 0.5f);
		CObjMgr::Get_Instance()->SetTerranRush(true);
	}
	if (CInputMgr::Get_Instance()->KeyDownVK(VK_F3))
	{
		CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingBirth.wav", 0.5f);
		CObjMgr::Get_Instance()->SetProtossRush(true);
	}
	if (CInputMgr::Get_Instance()->KeyDownVK(VK_F4))
	{
		//BGM 종료하기! -> 최종 전투 사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingBirth.wav", 0.5f);
		CObjMgr::Get_Instance()->SetLastRush(true);
	}
}

void CStage::CheckEndGame()
{
	//디버깅용
	if (CInputMgr::Get_Instance()->KeyPressVK(VK_F12))
	{
		CMainUI::Get_Instance()->ShowWinText(true);
	}
	int count = CObjMgr::Get_Instance()->GetDestroyCount();

	if (count >= 3) //타운홀 건물 3개 전부 다 파괴되었을 경우에 게임 종료되도록 설정
	{
		CMainUI::Get_Instance()->ShowWinText(true);
	}
}

void CStage::ClampMouse(HWND hWnd)
{
	RECT rc{};
	GetClientRect(hWnd, &rc);

	POINT lt{ rc.left, rc.top };
	POINT rb{ rc.right, rc.bottom };

	ClientToScreen(hWnd, &lt);
	ClientToScreen(hWnd, &rb);

	RECT clip{ lt.x, lt.y, rb.x, rb.y };
	ClipCursor(&clip);
}

CObj* CStage::FindObjectAtPosition(Vec2& worldPos)
{
	float searchRadius = 40.f; //클릭 인식 반경
	//모든 오브젝트 그룹 탐색
	for (int i = 0; i < OBJ_END; ++i)
	{
		list<CObj*>& objList = CObjMgr::Get_Instance()->Get_ObjList(OBJID(i));
		for (auto* pObj : objList)
		{
			if (!pObj || pObj->IsDead())
				continue;
			Vec2 objPos = pObj->Get_Pos();
			Vec2 diff = { worldPos.fX - objPos.fX, worldPos.fY - objPos.fY };
			float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);

			if (dist <= searchRadius)
			{
				return pObj;
			}
		}
	}
	return nullptr;
}

void CStage::CreateResource()
{
	for (int r = 0; r < TILEY; ++r)
	{
		for (int c = 0; c < TILEX; ++c)
		{
			CObj* pObj = CTileMgr::Get_Instance()->GetTile(r, c);
			if (!pObj)  // nullptr 체크
				continue;

			CTile* pTile = dynamic_cast<CTile*>(pObj);
			if (!pTile)
				continue;

			int iOption = pTile->Get_Option();
			if (iOption == 2)
			{
				// 타일 중앙 좌표 계산
				float centerX = c * 32.f + 16.f;
				float centerY = r * 32.f + 16.f;

				CMineral* pMineral = new CMineral();
				pMineral->Initialize();
				pMineral->Set_Pos(centerX, centerY);
				CObjMgr::Get_Instance()->Add_Object(OBJ_RESOURCE, pMineral);
			}
			else if (iOption == 3)
			{
				// 타일 중앙 좌표 계산
				float centerX = c * 32.f + 16.f;
				float centerY = r * 32.f + 16.f;

				CGas* pGas = new CGas();
				pGas->Initialize();
				pGas->Set_Pos(centerX, centerY);
				CObjMgr::Get_Instance()->Add_Object(OBJ_RESOURCE, pGas);
			}
		}
	}
}

void CStage::SpawnProtossBase()
{
	////다크 아칸 생성
	CObj* pDarkArchon = CAbstractFactory<CDarkArchon>::Create(400.f, 200.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDarkArchon);
	//프로브 생성
	CObj* pProbe = CAbstractFactory<CProbe>::Create(400.f, 250.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pProbe);
	//적 SCV 생성
	CObj* pSCV = CAbstractFactory<CSCV>::Create(400.f, 300.f);
	pSCV->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pSCV);
	//적 드론 생성
	//CObj* pDrone = CAbstractFactory<CDrone>::Create(400.f, 350.f);
	//pDrone->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pDrone);
	//넥서스 생성
	//CObj* pNexus = CAbstractFactory<CNexus>::Create(296.f, 236.f);
	//CNexus* ppNexus = dynamic_cast<CNexus*>(pNexus);
	//ppNexus->SetGhost(false); ppNexus->SetState(eBuildingState::CONSTRUCT);
	//ppNexus->SetHP(ppNexus->Get_MaxHP()); ppNexus->AppplyOccupy();
	//ppNexus->SetFrameKey(L"Nexus");
	//ppNexus->SetTeamType(eTeamType::ALLY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pNexus);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pProbe);
	//pProbe = CAbstractFactory<CProbe>::Create(300.f, 300.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pProbe);
	//pProbe = CAbstractFactory<CProbe>::Create(320.f, 300.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pProbe);
	//적 드론 생성
	//CObj* pDrone = CAbstractFactory<CDrone>::Create(200.f, 600.f);
	//pDrone->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pDrone);
	////질럿 생성
	//CObj* pZealot = CAbstractFactory<CZealot>::Create(300.f, 300.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pZealot);
	////셔틀 생성
	//CObj* pShuttle = CAbstractFactory<CShuttle>::Create(500.f, 300.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pShuttle);
	////아칸 생성
	////CObj* pArchon = CAbstractFactory<CArchon>::Create(300.f, 100.f);
	////CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pArchon);
	//pDrone = CAbstractFactory<CDrone>::Create(500.f, 500.f);
	//pDrone->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pDrone);
	//라바 생성
	//CObj* pLava = CAbstractFactory<CLava>::Create(300.f, 500.f);
	//pLava->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pLava);
	//pLava = CAbstractFactory<CLava>::Create(300.f, 550.f);
	//pLava->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pLava);
	//pLava = CAbstractFactory<CLava>::Create(300.f, 600.f);
	//pLava->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pLava);
	//pLava = CAbstractFactory<CLava>::Create(300.f, 650.f);
	//pLava->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pLava);
	//pLava = CAbstractFactory<CLava>::Create(300.f, 700.f);
	//pLava->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pLava);
	////다크 템플러 생성
	//CObj* pDarkTemplar = CAbstractFactory<CDarkTemplar>::Create(300.f, 500.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDarkTemplar);
	//pDarkTemplar = CAbstractFactory<CDarkTemplar>::Create(200.f, 500.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDarkTemplar);
	////드라군 생성
	//CObj* pDragon = CAbstractFactory<CDragon>::Create(200.f, 400.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDragon);
	////드라군 생성
	//pDragon = CAbstractFactory<CDragon>::Create(300.f, 400.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDragon);
	////아비터 생성
	//CObj* pArbiter = CAbstractFactory<CArbiter>::Create(400.f, 100.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pArbiter);
	////캐리어 생성
	//CObj* pCarrier = CAbstractFactory<CCarrier>::Create(600.f, 400.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pCarrier);
	//pCarrier = CAbstractFactory<CCarrier>::Create(700.f, 400.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pCarrier);
	////커세어 생성
	//CObj* pCorsair = CAbstractFactory<CCorsair>::Create(200.f, 100.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pCorsair);
	////하이템플러 생성
	//CObj* pHighTemplar = CAbstractFactory<CHighTemplar>::Create(600.f, 200.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pHighTemplar);
	////리버 생성
	//CObj* pReavor = CAbstractFactory<CReavor>::Create(700.f, 300.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pReavor);
	////스카웃 생성
	//CObj* pScout = CAbstractFactory<CScout>::Create(100.f, 100.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pScout);
	////옵저버 생성
	//CObj* pObserver = CAbstractFactory<CObserver>::Create(500.f, 400.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pObserver);
	////적 마린 생성
	//CObj* pMarine = CAbstractFactory<CMarine>::Create(200.f, 500.f);
	//pMarine->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pMarine);
	////적 배틀크루저 생성
	//CObj* pBattleCruiser = CAbstractFactory<CBattleCruiser>::Create(200.f, 600.f);
	//pBattleCruiser->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pBattleCruiser);
	////적 탱크 생성
	//CObj* pTank = CAbstractFactory<CTank>::Create(400.f, 600.f);
	//pTank->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pTank);
	////적 벌쳐 생성
	//CObj* pVulture = CAbstractFactory<CVulture>::Create(500.f, 600.f);
	//pVulture->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pVulture);
	////적 골리앗 생성
	//CObj* pGoliath = CAbstractFactory<CGoliath>::Create(600.f, 600.f);
	//pGoliath->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pGoliath);
	SpawnEnemyBase();
}

void CStage::SpawnEnemyBase()
{
	SpawnEnemyTerranBase();
	SpawnEnemyProtossBase();
	SpawnEnemyZergBase();
}

void CStage::SpawnEnemyTerranBase()
{
	//커맨드 센터 생성
	CObj* pCommandCenter = CAbstractFactory<CCommandCenter>::Create(1300.f, 1800.f);
	CCommandCenter* ppCommandCenter = dynamic_cast<CCommandCenter*>(pCommandCenter);
	ppCommandCenter->SetGhost(false); ppCommandCenter->SetState(eBuildingState::CONSTRUCT);
	ppCommandCenter->SetHP(ppCommandCenter->Get_MaxHP()); ppCommandCenter->AppplyOccupy();
	ppCommandCenter->SetFrameKey(L"CommandCenter");
	ppCommandCenter->SetTeamType(eTeamType::ENEMY);
	ppCommandCenter->SetSelectable(false);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, ppCommandCenter);
	//배럭 생성
	//CObj* pBarracks = CAbstractFactory<CBarracks>::Create(1100.f, 1650.f);
	//CBarracks* ppBarracks = dynamic_cast<CBarracks*>(pBarracks);
	//ppBarracks->SetGhost(false); ppBarracks->SetState(eBuildingState::CONSTRUCT);
	//ppBarracks->SetHP(ppBarracks->Get_MaxHP()); ppBarracks->AppplyOccupy();
	//ppBarracks->SetFrameKey(L"BARRACKS_ANIM");
	//ppBarracks->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, ppBarracks);
	////군수공장 생성
	//CObj* pFactory = CAbstractFactory<CFactory>::Create(1200.f, 1700.f);
	//CFactory* ppFactory = dynamic_cast<CFactory*>(pFactory);
	//ppFactory->SetGhost(false); ppFactory->SetState(eBuildingState::CONSTRUCT);
	//ppFactory->SetHP(ppFactory->Get_MaxHP()); ppFactory->AppplyOccupy();
	//ppFactory->SetFrameKey(L"FACTORY_ANIM");
	//ppFactory->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, ppFactory);
	////스타포트 생성
	//CObj* pStarport = CAbstractFactory<CStarport>::Create(1000.f, 1600.f);
	//CStarport* ppStarport = dynamic_cast<CStarport*>(pStarport);
	//ppStarport->SetGhost(false); ppStarport->SetState(eBuildingState::CONSTRUCT);
	//ppStarport->SetHP(ppStarport->Get_MaxHP()); ppStarport->AppplyOccupy();
	//ppStarport->SetFrameKey(L"STARPORT_ANIM");
	//ppStarport->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, ppStarport);
}

void CStage::SpawnEnemyProtossBase()
{
	//넥서스 생성
	CObj* pNexus = CAbstractFactory<CNexus>::Create(1750.f, 1600.f);
	CNexus* ppNexus = dynamic_cast<CNexus*>(pNexus);
	ppNexus->SetGhost(false); ppNexus->SetState(eBuildingState::CONSTRUCT);
	ppNexus->SetHP(ppNexus->Get_MaxHP()); ppNexus->AppplyOccupy();
	ppNexus->SetFrameKey(L"Nexus");
	ppNexus->SetTeamType(eTeamType::ENEMY);
	ppNexus->SetSelectable(false);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, ppNexus);
	////관문 생성
	//CObj* pGate = CAbstractFactory<CGateway>::Create(1600.f, 1350.f);
	//CGateway* ppGate = dynamic_cast<CGateway*>(pGate);
	//ppGate->SetGhost(false); ppGate->SetState(eBuildingState::CONSTRUCT);
	//ppGate->SetHP(ppGate->Get_MaxHP()); ppGate->AppplyOccupy();
	//ppGate->SetFrameKey(L"Gateway");
	//ppGate->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pGate);
	////로보틱스 생성
	//CObj* pRobotics = CAbstractFactory<CRoboticsFacility>::Create(1500.f, 1300.f);
	//CRoboticsFacility* ppRobotics = dynamic_cast<CRoboticsFacility*>(pRobotics);
	//ppRobotics->SetGhost(false); ppRobotics->SetState(eBuildingState::CONSTRUCT);
	//ppRobotics->SetHP(ppRobotics->Get_MaxHP()); ppRobotics->AppplyOccupy();
	//ppRobotics->SetFrameKey(L"Robotics_Facility");
	//ppRobotics->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pRobotics);
	////스타게이트 생성
	//CObj* pStargate = CAbstractFactory<CStargate>::Create(1700.f, 1400.f);
	//CStargate* ppStargate = dynamic_cast<CStargate*>(pStargate);
	//ppStargate->SetGhost(false); ppStargate->SetState(eBuildingState::CONSTRUCT);
	//ppStargate->SetHP(ppStargate->Get_MaxHP()); ppStargate->AppplyOccupy();
	//ppStargate->SetFrameKey(L"Stargate");
	//ppStargate->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pStargate);
}

void CStage::SpawnEnemyZergBase()
{
	//하이브 생성
	CObj* pHive = CAbstractFactory<CHive>::Create(1550.f, 1700.f);
	CHive* ppHive = dynamic_cast<CHive*>(pHive);
	ppHive->SetGhost(false); ppHive->SetState(eBuildingState::CONSTRUCT);
	ppHive->SetHP(ppHive->Get_MaxHP()); ppHive->AppplyOccupy();
	ppHive->SetFrameKey(L"HIVE");
	ppHive->SetTeamType(eTeamType::ENEMY);
	ppHive->SetSelectable(false);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pHive);
	////스파이어 생성
	//CObj* pSpire = CAbstractFactory<CSpire>::Create(1500.f, 1650.f);
	//CSpire* ppSpire = dynamic_cast<CSpire*>(pSpire);
	//ppSpire->SetGhost(false); ppSpire->SetState(eBuildingState::CONSTRUCT);
	//ppSpire->SetHP(ppSpire->Get_MaxHP()); ppSpire->AppplyOccupy();
	//ppSpire->SetFrameKey(L"SPIRE");
	//ppSpire->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pSpire);
	////히드라리스크덴 생성
	//CObj* pHydraliskDen = CAbstractFactory<CHydraliskDen>::Create(1400.f, 1700.f);
	//CHydraliskDen* ppHydraliskDen = dynamic_cast<CHydraliskDen*>(pHydraliskDen);
	//ppHydraliskDen->SetGhost(false); ppHydraliskDen->SetState(eBuildingState::CONSTRUCT);
	//ppHydraliskDen->SetHP(ppHydraliskDen->Get_MaxHP()); ppHydraliskDen->AppplyOccupy();
	//ppHydraliskDen->SetFrameKey(L"HYDRALISK_DEN");
	//ppHydraliskDen->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pHydraliskDen);
	////스포닝풀 생성
	//CObj* pSpawningPool = CAbstractFactory<CSpawningPool>::Create(1600.f, 1600.f);
	//CSpawningPool* ppSpawningPool = dynamic_cast<CSpawningPool*>(pSpawningPool);
	//ppSpawningPool->SetGhost(false); ppSpawningPool->SetState(eBuildingState::CONSTRUCT);
	//ppSpawningPool->SetHP(ppSpawningPool->Get_MaxHP());
	//ppSpawningPool->AppplyOccupy();
	//ppSpawningPool->SetFrameKey(L"SPAWNING_POOL");
	//ppSpawningPool->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pSpawningPool);
	////울트라리스크덴 생성
	//CObj* pUltraliskDen = CAbstractFactory<CUltraliskDen>::Create(1650.f, 1500.f);
	//CUltraliskDen* ppUltraliskDen = dynamic_cast<CUltraliskDen*>(pUltraliskDen);
	//ppUltraliskDen->SetGhost(false); ppUltraliskDen->SetState(eBuildingState::CONSTRUCT);
	//ppUltraliskDen->SetHP(ppUltraliskDen->Get_MaxHP());
	//ppUltraliskDen->AppplyOccupy();
	//ppUltraliskDen->SetFrameKey(L"ULTRALISK_DEN");
	//ppUltraliskDen->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pUltraliskDen);
}

void CStage::SpawnZergBase()
{
	//하이브 생성
	CObj* pHive = CAbstractFactory<CHive>::Create(292.f, 236.f);
	CHive* ppHive = dynamic_cast<CHive*>(pHive);
	ppHive->SetGhost(false); ppHive->SetState(eBuildingState::CONSTRUCT);
	ppHive->SetHP(ppHive->Get_MaxHP()); ppHive->AppplyOccupy();
	ppHive->SetFrameKey(L"HIVE");
	ppHive->SetTeamType(eTeamType::ALLY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pHive);
	//드론 생성
	CObj* pDrone = CAbstractFactory<CDrone>::Create(100.f, 100.f);
	pDrone->SetTeamType(eTeamType::ALLY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDrone);
	pDrone = CAbstractFactory<CDrone>::Create(20.f, 20.f);
	pDrone->SetTeamType(eTeamType::ALLY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDrone);
	//저글링 생성
	CObj* pZergling = CAbstractFactory<CZergling>::Create(400.f, 100.f);
	pDrone->SetTeamType(eTeamType::ALLY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pZergling);
	//오버로드 생성
	CObj* pOverload = CAbstractFactory<COverload>::Create(100.f, 400.f);
	pOverload->SetTeamType(eTeamType::ALLY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pOverload);
	pOverload = CAbstractFactory<COverload>::Create(100.f, 200.f);
	pOverload->SetTeamType(eTeamType::ALLY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pOverload);
	//뮤탈리스크 생성
	CObj* pMutalisk = CAbstractFactory<CMutalisk>::Create(100.f, 5000.f);
	pMutalisk->SetTeamType(eTeamType::ALLY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMutalisk);
	//적 히드라리스크 생성
	CObj* pHydralisk = CAbstractFactory<CHydralisk>::Create(280.f, 1000.f);
	pHydralisk->SetTeamType(eTeamType::ALLY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pHydralisk);
	pHydralisk = CAbstractFactory<CHydralisk>::Create(280.f, 1000.f);
	pHydralisk->SetTeamType(eTeamType::ALLY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pHydralisk);
	//적 울트라리스크 생성
	CObj* pUltralisk = CAbstractFactory<CUltralisk>::Create(140.f, 300.f);
	pUltralisk->SetTeamType(eTeamType::ALLY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pUltralisk);
	pUltralisk = CAbstractFactory<CUltralisk>::Create(200.f, 600.f);
	pUltralisk->SetTeamType(eTeamType::ALLY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pUltralisk);
}

void CStage::CreateZergBuildings()
{
	//하이브 생성
	CObj* pHive = CAbstractFactory<CHive>::Create(1400.f, 1600.f);
	CHive* ppHive = dynamic_cast<CHive*>(pHive);
	ppHive->SetGhost(false); ppHive->SetState(eBuildingState::CONSTRUCT);
	ppHive->SetHP(ppHive->Get_MaxHP()); ppHive->AppplyOccupy();
	ppHive->SetFrameKey(L"HIVE");
	ppHive->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pHive);
	//히드라리스크덴 생성
	CObj* pHydraliskDen = CAbstractFactory<CHydraliskDen>::Create(1500.f, 1600.f);
	CHydraliskDen* ppHydraliskDen = dynamic_cast<CHydraliskDen*>(pHydraliskDen);
	ppHydraliskDen->SetGhost(false); ppHydraliskDen->SetState(eBuildingState::CONSTRUCT);
	ppHydraliskDen->SetHP(ppHive->Get_MaxHP()); ppHydraliskDen->AppplyOccupy();
	ppHydraliskDen->SetFrameKey(L"HYDRALISK_DEN");
	ppHydraliskDen->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pHydraliskDen);
	//스파이어 생성
	CObj* pSpire = CAbstractFactory<CSpire>::Create(1300.f, 1600.f);
	CSpire* ppSpire = dynamic_cast<CSpire*>(pSpire);
	ppSpire->SetGhost(false); ppSpire->SetState(eBuildingState::CONSTRUCT);
	ppSpire->SetHP(ppHive->Get_MaxHP()); ppSpire->AppplyOccupy();
	ppSpire->SetFrameKey(L"SPIRE");
	ppSpire->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pSpire);
	//스포닝풀 생성
	CObj* pSpawningPool = CAbstractFactory<CSpawningPool>::Create(1400.f, 1500.f);
	CSpawningPool* ppSpawningPool = dynamic_cast<CSpawningPool*>(pSpawningPool);
	ppSpawningPool->SetGhost(false); ppSpawningPool->SetState(eBuildingState::CONSTRUCT);
	ppSpawningPool->SetHP(ppSpawningPool->Get_MaxHP());
	ppSpawningPool->AppplyOccupy();
	ppSpawningPool->SetFrameKey(L"SPAWNING_POOL");
	ppSpawningPool->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pSpawningPool);
	//울트라리스크덴 생성
	CObj* pUltraliskDen = CAbstractFactory<CUltraliskDen>::Create(1400.f, 1700.f);
	CUltraliskDen* ppUltraliskDen = dynamic_cast<CUltraliskDen*>(pUltraliskDen);
	ppUltraliskDen->SetGhost(false); ppUltraliskDen->SetState(eBuildingState::CONSTRUCT);
	ppUltraliskDen->SetHP(ppUltraliskDen->Get_MaxHP());
	ppUltraliskDen->AppplyOccupy();
	ppUltraliskDen->SetFrameKey(L"ULTRALISK_DEN");
	ppUltraliskDen->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pUltraliskDen);
}

void CStage::CreateTerrranEnemyTest()
{
	//하이브 생성
	CObj* pBlueCC = CAbstractFactory<CCommandCenter>::Create(600.f, 600.f);
	CCommandCenter* pBlueComC = dynamic_cast<CCommandCenter*>(pBlueCC);

	//커맨드 센터 초기화
	pBlueComC->SetGhost(false); pBlueComC->SetState(eBuildingState::CONSTRUCT);
	pBlueComC->SetHP(pBlueComC->Get_MaxHP()); pBlueComC->AppplyOccupy();
	pBlueComC->SetFrameKey(L"Blue_CommandCenter");
	//적으로 설정
	pBlueComC->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pBlueComC);

	//CObj* pBlueMarine = CAbstractFactory<CBlueMarine>::Create(300.f, 900.f);
	//pBlueMarine->SetTeamType(eTeamType::ENEMY); //적으로 설정
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pBlueMarine);

	//적 마린 생성
	for (int r = 0; r < 2; ++r)  // 2행
	{
		for (int c = 0; c < 2; ++c)  // 2열
		{
			CObj* pBlueMarine = CAbstractFactory<CBlueMarine>::Create(
				280.f + c * 80.f,  // X 간격 80
				850.f + r * 80.f  // Y 간격 80
			);
			pBlueMarine->SetTeamType(eTeamType::ENEMY); //적으로 설정
			CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pBlueMarine);
		}
	}
}

void CStage::CreateZergEnemyTest()
{
	//적 오버로드 생성
	CObj* pOverload = CAbstractFactory<COverload>::Create(100.f, 800.f);
	pOverload->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pOverload);
	pOverload = CAbstractFactory<COverload>::Create(100.f, 200.f);
	pOverload->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pOverload);

	//적 뮤탈리스크 생성
	CObj* pMutalisk = CAbstractFactory<CMutalisk>::Create(100.f, 1000.f);
	pMutalisk->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pMutalisk);

	//적 질럿 생성
	//CObj* pZealot = CAbstractFactory<CZealot>::Create(100.f, 700.f);
	//pZealot->SetTeamType(eTeamType::ENEMY);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pZealot);

	//적 저글링 생성
	CObj* pZergling = CAbstractFactory<CZergling>::Create(280.f, 900.f);
	pZergling->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pZergling);
	pZergling = CAbstractFactory<CZergling>::Create(280.f, 900.f);
	pZergling->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pZergling);

	//적 히드라리스크 생성
	CObj* pHydralisk = CAbstractFactory<CHydralisk>::Create(280.f, 1000.f);
	pHydralisk->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pHydralisk);
	pHydralisk = CAbstractFactory<CHydralisk>::Create(280.f, 1000.f);
	pHydralisk->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pHydralisk);

	//적 울트라리스크 생성
	CObj* pUltralisk = CAbstractFactory<CUltralisk>::Create(140.f, 900.f);
	pUltralisk->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pUltralisk);

	pUltralisk = CAbstractFactory<CUltralisk>::Create(200.f, 900.f);
	pUltralisk->SetTeamType(eTeamType::ENEMY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pUltralisk);
}

void CStage::CreateObject()
{
	//마린 3마리 생성
	CObj* pMarin = CAbstractFactory<CMarine>::Create(300.f, 300.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMarin);
	pMarin = CAbstractFactory<CMarine>::Create(400.f, 300.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMarin);
	pMarin = CAbstractFactory<CMarine>::Create(500.f, 300.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMarin);

	//배틀크루저 생성
	CObj* pBattleCruiser = CAbstractFactory<CBattleCruiser>::Create(600.f, 400.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pBattleCruiser);
	pBattleCruiser = CAbstractFactory<CBattleCruiser>::Create(500.f, 400.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pBattleCruiser);
	pBattleCruiser = CAbstractFactory<CBattleCruiser>::Create(700.f, 400.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pBattleCruiser);

	//탱크 생성
	CObj* pTank = CAbstractFactory<CTank>::Create(200.f, 500.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pTank);
	pTank = CAbstractFactory<CTank>::Create(300.f, 500.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pTank);
	pTank = CAbstractFactory<CTank>::Create(100.f, 500.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pTank);

	//SCV 생성
	CObj* pSCV = CAbstractFactory<CSCV>::Create(100.f, 100.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	pSCV = CAbstractFactory<CSCV>::Create(200.f, 100.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	pSCV = CAbstractFactory<CSCV>::Create(300.f, 100.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);

	//메딕 생성
	CObj* pMedic = CAbstractFactory<CMedic>::Create(400.f, 200.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMedic);

	//파이어벳 생성
	CObj* pFirebat = CAbstractFactory<CFirebat>::Create(600.f, 400.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pFirebat);
	pFirebat = CAbstractFactory<CFirebat>::Create(600.f, 500.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pFirebat);

	//고스트 생성
	CObj* pGhost = CAbstractFactory<CGhost>::Create(400.f, 400.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pGhost);
	pGhost = CAbstractFactory<CGhost>::Create(350.f, 500.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pGhost);
	pGhost = CAbstractFactory<CGhost>::Create(300.f, 500.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pGhost);

	//벌쳐 생성
	CObj* pVulture = CAbstractFactory<CVulture>::Create(500.f, 200.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pVulture);

	//골리앗 생성
	CObj* pGoliath = CAbstractFactory<CGoliath>::Create(500.f, 300.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pGoliath);

	//커맨드 센터 생성
	CObj* pCC = CAbstractFactory<CCommandCenter>::Create(292.f, 236.f);
	CCommandCenter* pComC = dynamic_cast<CCommandCenter*>(pCC);
	//커맨드 센터 초기화
	pComC->SetGhost(false); pComC->SetState(eBuildingState::CONSTRUCT);
	pComC->SetHP(pComC->Get_MaxHP()); pComC->AppplyOccupy();
	CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pCC);
}



void CStage::LoadTerranResource()
{
	//스테이지 이미지
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MyFolder/Map/FightSpirit.bmp", L"Stage");
	//마린 스프라이트 시트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Marine/Marine.bmp", L"Marine");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Marine/MarineGunSpark.bmp", L"MarineHit");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Marine/MarineDeathEffect.bmp", L"MARINE_DEATH_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Marine/Marine.bmp", L"MARINE_PORTRAIT");
	//배틀크루저 스프라이트 시트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/BattleCruiser/BattleCruiser.bmp", L"BattleCruiser");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/BattleCrusor/BattleAttack.bmp", L"BCBullet");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/BattleCrusor/Yamato.bmp", L"BCYamato");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Battlecrusor/BattleCrusor.bmp", L"BATTLECRUISER_PORTRAIT");
	//탱크 스프라이트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Tank/TankBody.bmp", L"Tank_Body");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Tank/TankHead.bmp", L"Tank_Head");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/TankAtt/TankHit.bmp", L"TankHit");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/TankAtt/SiegeTankHit.bmp", L"SiegeTankHit");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Tank/Tank.bmp", L"TANK_PORTRAIT");
	//SCV
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/SCV/SCV.bmp", L"SCV");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/SCV/SCVEffect.bmp", L"SCV_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/SCV/SCV.bmp", L"SCV_PORTRAIT");
	//Medic
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Medic/Medic.bmp", L"Medic");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Medic/MedicDeathEffect.bmp", L"MEDIC_DEATH_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Medic/Medic.bmp", L"MEDIC_PORTRAIT");
	//Vulture
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Vulture/Vulture.bmp", L"Vulture");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Bullet/VultureAtt/grenade0.png", L"VBullet");
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szKey[128];
		TCHAR szPath[256];

		wsprintf(szKey, L"Vulture_Portrait_%d", r);
		wsprintf(szPath, L"../Image/UI/Portrait/Vulture/%d.bmp", r);

		CBmpMgr::Get_Instance()->Insert_Bmp_Anim(szPath, szKey);
	}
	//Goliath
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Goliath/GoliathBody.bmp", L"Goliath_Body");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Goliath/GoliathHead.bmp", L"Goliath_Head");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Bullet/MissileAtt/missile0.png", L"GBullet");
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Bullet/BomberAtt/gemini%d.png", r);
		wsprintf(szKey, L"Missile_%d", r);
		CBmpMgr::Get_Instance()->Insert_Bmp(szPath, szKey);
	}
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szKey[128];
		TCHAR szPath[256];

		wsprintf(szKey, L"Goliath_Portrait_%d", r);
		wsprintf(szPath, L"../Image/UI/Portrait/Goliath/%d.bmp", r);

		CBmpMgr::Get_Instance()->Insert_Bmp_Anim(szPath, szKey);
	}
	//Firebat
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/FireBat/Firebat.bmp", L"FIREBAT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/FireBat/FireBatHit.bmp", L"FIREBAT_HIT_EFFECT");
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szKey[128];
		TCHAR szPath[256];

		wsprintf(szKey, L"FireBat_Portrait_%d", r);
		wsprintf(szPath, L"../Image/UI/Portrait/FireBat/%d.bmp", r);

		CBmpMgr::Get_Instance()->Insert_Bmp_Anim(szPath, szKey);
	}
	//Ghost
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Ghost/Ghost.bmp", L"GHOST");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Ghost/GhostShot.bmp", L"GHOST_SHOT_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Ghost/GhostDeathEffect.bmp", L"GHOST_DEATH_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/NukeMissile/NuclearMissleDown.bmp", L"GHOST_NUCLEAR_BULLET");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Nuclear/NuclearEffect.bmp", L"GHOST_NUCLEAR_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Ghost/Ghost.bmp", L"GHOST_PORTRAIT");
	//커맨드 센터
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/CommandCenter/CommandCenter.bmp", L"CommandCenter");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/CommandCenter.bmp", L"COMMANDCENTER_CONSTRUCT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Advisor/Advisor.bmp", L"ADVISOR_PORTRAIT");
	//블루 커맨드 센터 
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/trash/Enemy/Blue_CommandCenter.bmp", L"Blue_CommandCenter");
	//블루 마린
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/E_Unit/MarineBlue.bmp", L"BLUE_MARINE");
	//배럭
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Barrack/Barrack.bmp", L"Barracks");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Barrack.bmp", L"BARRACKS_ANIM");
	//스타포트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Starport/Starport.bmp", L"Starport");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Starport/StarportAddOn.bmp", L"STARPORT_ADDON");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Starport.bmp", L"STARPORT_ANIM");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/StarportAddOn.bmp", L"STARPORT_ADDON_ANIM");
	//보급고
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/SupplyDepot/SupplyDepot.bmp", L"SupplyDepot");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/SupplyDepot.bmp", L"SUPPLYDEPOT_ANIM");
	//팩토리
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Factory/Factory.bmp", L"Factory");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Factory/FactoryAddOn.bmp", L"FACTORY_ADDON");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Factory.bmp", L"FACTORY_ANIM");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/FactoryAddOn.bmp", L"FACTORY_ADDON_ANIM");
	//정제소
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Refinery/Refinery4.bmp", L"Refinery");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Refinery.bmp", L"Refinery_Anim");
	//벙커
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Bunker/Bunker.bmp", L"BUNKER");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Bunker.bmp", L"BUNKER_ANIM");
	//터렛
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Turret/Turret.bmp", L"TURRET");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Turret.bmp", L"TURRET_ANIM");
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Bullet/TurretAtt/hks%d.png", r);
		wsprintf(szKey, L"Turret_Missile_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//엔지니어링베이
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/EngineeringBay/EngineeringBay.bmp", L"ENGINEERING_BAY");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/EngineeringBay.bmp", L"ENGINEERING_BAY_ANIM");
	//아머리
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Armory/Armory.bmp", L"ARMORY");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Armory.bmp", L"ARMORY_ANIM");
	//아카데미
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Academy/Academy.bmp", L"ACADEMY");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Academy.bmp", L"ACADEMY_ANIM");
	//Scienece Facility
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Facility/ScienceFacility.bmp", L"SCIENCE_FACILITY");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Facility/SciencePhysics2.bmp", L"SCIENCE_PHYSICS");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Facility/ScienceSecret.bmp", L"SCIENCE_SECRET");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/ScienceFacility.bmp", L"SCIENCE_FACILITY_ANIM");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/SciencePhysics.bmp", L"SCIENCE_PHYSICS_ANIM");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/ScienceSecret.bmp", L"SCIENCE_SECRET_ANIM");
	//빌딩 템플릿 
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildTemplate.bmp", L"BUILD_TEMPLATE");
	//미네랄, 가스 
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Resource/Mineral0.bmp", L"Mineral");
	//CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Resource/mineral.bmp", L"Mineral");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Resource/mineral.png", L"MINERAL");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Resource/miniGas.png", L"GAS");
	//Wintext
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Win/WinText.bmp", L"WIN_TEXT");
}

void CStage::LoadZergResource()
{
	//저그 크립
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Single/Map/0.png", L"Crip");

	//하이브
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Hive/0.png", L"HIVE");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Hive/Wire/0.png", L"HIVE_WIRE");
	//레어
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Unit/Build/lair/0.png", L"Lare");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/lair/0.png", L"Lare_Wire");
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Hive/Portrait/%d.png", r);
		wsprintf(szKey, L"Hive_Portrait_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//드론 move
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 3; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/ZergRes/Texture/Multi/Unit/Drone/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Drone_Move%d_Anim%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//Zerg Unit Big WireFrame
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/Drone/0.png", L"Drone_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/lava/0.png", L"Lava_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/Zergling/0.png", L"Zergling_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/Overload/0.png", L"Overload_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/Hydralisk/0.png", L"Hydralisk_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/Mutalisk/0.png", L"Mutalisk_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/Ultralisk/0.png", L"Ultralisk_BigWire");

	//Zerg Unit Small WireFrame
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/SmallFrames/Drone/0.png", L"Drone_SmallWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/SmallFrames/lava/0.png", L"Lava_SmallWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/SmallFrames/Zergling/0.png", L"Zergling_SmallWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/SmallFrames/Overload/0.png", L"Overload_SmallWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/SmallFrames/Hydralisk/0.png", L"Hydralisk_SmallWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/SmallFrames/Mutalisk/0.png", L"Mutalisk_SmallWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/SmallFrames/Ultralisk/0.png", L"Ultralisk_SmallWire");

	//Zerg Building Big WireFrame
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/Hatchery/0.png", L"Hatchery_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/lair/0.png", L"Lair_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/hive/0.png", L"Hive_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/pool/0.png", L"SpawningPool_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/base_Hydralisk/0.png", L"HydraliskDen_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/spire/0.png", L"Spire_BigWire");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/base_Ultralisk/0.png", L"UltraliskDen_BigWire");

	//라바 Birth
	for (int r = 0; r < 8; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/BuildDeath/%d.png", r);
		wsprintf(szKey, L"BUILDING_DESTROY_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//저그 건물
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Unit/Build/Hatchery/0.png", L"Hatchery");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Frames/BigFrames/Hatchery/0.png", L"Hatchery_BigFrame");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/ZergRes/Texture/Multi/Unit/Build/base_queen/0.png", L"BaseQueen");
	//히드라리스크덴
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/HydraliskDen/0.png", L"HYDRALISK_DEN");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/HydraliskDen/Wire/0.png", L"HYDRALISK_DEN_WIRE");
	//스파이어
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Spire/0.png", L"SPIRE");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Spire/Wire/0.png", L"SPIRE_WIRE");
	//스포닝풀
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/SpawningPool/0.png", L"SPAWNING_POOL");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/SpawningPool/Wire/0.png", L"SPAWNING_POOL_WIRE");
	//울트라리스크덴
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Build_Ultralisk/0.png", L"ULTRALISK_DEN");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Build_Ultralisk/Wire/0.png", L"ULTRALISK_DEN_WIRE");
	//Lava Birth
	for (int i = 0; i < 8; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/ZergRes/Texture/Multi/Unit/lava/Birth/%d.png", i);
		wsprintf(szKey, L"Lava_Birth%d", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Lava Move
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 5; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/ZergRes/Texture/Multi/Unit/lava/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Lava_Move%d_Anim%d", r, c);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//저그 빌드 애니메이션
	for (int r = 0; r < 18; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/ZergRes/Texture/Multi/Unit/Build/CreateNormal/%d.png", r);
		wsprintf(szKey, L"Zerg_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//저그 Active 버튼
	for (int i = 0; i < 14; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Active/%d.png", i);
		wsprintf(szKey, L"Zerg_Active%d", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);

		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Active/%d_0.png", i);
		wsprintf(szKey, L"Zerg_Active%d_0", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//저그 Build 버튼
	for (int i = 0; i < 20; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Z_Build/%d.png", i);
		wsprintf(szKey, L"Zerg_Build%d", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);

		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Z_Build/%d_0.png", i);
		wsprintf(szKey, L"Zerg_Build%d_0", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//저그 유닛 Air 
	for (int i = 0; i < 7; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Z_Air/%d.png", i);
		wsprintf(szKey, L"Zerg_Air%d", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);

		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Z_Air/%d_0.png", i);
		wsprintf(szKey, L"Zerg_Air%d_0", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//저그 유닛 Ground 
	for (int i = 0; i < 20; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Z_Ground/%d.png", i);
		wsprintf(szKey, L"Zerg_Ground%d", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);

		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Z_Ground/%d_0.png", i);
		wsprintf(szKey, L"Zerg_Ground%d_0", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//저그 Skill 버튼
	for (int i = 0; i < 8; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Z_Skill/%d.png", i);
		wsprintf(szKey, L"Zerg_Skill%d", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);

		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Z_Skill/%d_0.png", i);
		wsprintf(szKey, L"Zerg_Skill%d_0", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//저그 Upgrade 버튼
	for (int i = 0; i < 28; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Z_Upgrade/%d.png", i);
		wsprintf(szKey, L"Zerg_Upgrade%d", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);

		wsprintf(szPath, L"../Image/ZergRes/Texture/Single/Icon/Z_Upgrade/%d_0.png", i);
		wsprintf(szKey, L"Zerg_Upgrade%d_0", i);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//오버로드 
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 4; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Zerg/Overload/Move/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Overload_Move%d_Anim%d", r, c);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Overload/Wire/0.png", L"OVERLOAD_WIRE");
	//뮤탈리스크 move
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 5; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Zerg/Mutalisk/Move/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Mutalisk_Move%d_Anim%d", r, c);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//뮤탈리스크 Hit effect
	for (int r = 0; r < 10; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Mutalisk/HitEffect/%d.png", r);
		wsprintf(szKey, L"Mutalisk_HitEffect_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//뮤탈리스크 portrait
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Mutalisk/Portrait/%d.png", r);
		wsprintf(szKey, L"Mutalisk_Portrait_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//뮤탈리스크 Bullet
	for (int r = 0; r < 10; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Mutalisk/AttackBullet/%d.png", r);
		wsprintf(szKey, L"Mutalisk_Bullet_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Mutalisk/Wire/0.png", L"MUTALISK_WIRE");
	// 저글링 Attack
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 4; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Zerg/Zergling/Attack/Attack%d/%d.png", r, c);
			wsprintf(szKey, L"Zergling_Attack%d_Anim%d", r, c);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//저글링 portrait
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Zergling/Portrait/%d.png", r);
		wsprintf(szKey, L"Zergling_Portrait_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	// 저글링 Move
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 8; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Zerg/Zergling/Move/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Zergling_Move%d_Anim%d", r, c);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Zergling/Wire/0.png", L"ZERGLING_WIRE");
	//히드라 Attack
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 5; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/Zerg/Hydralisk/Attack/Attack%d/%d.png", r, c);
			wsprintf(szKey, L"Hydralisk_Attack%d_Anim%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//히드라 portrait
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Hydralisk/Portrait/%d.png", r);
		wsprintf(szKey, L"Hydralisk_Portrait_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//히드라 Move
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 7; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/Zerg/Hydralisk/Move/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Hydralisk_Move%d_Anim%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//히드라 Attack Bullet
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 7; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/Zerg/Hydralisk/AttackBullet/Attack%d/%d.png", r, c);
			wsprintf(szKey, L"Hydralisk_Attack%d_Bullet%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//히드라 hit effect
	for (int r = 0; r < 8; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/Zerg/Hydralisk/HitEffect/%d.png", r);
		wsprintf(szKey, L"Hydralisk_HitEffect_%d", r);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Hydralisk/Wire/0.png", L"HYDRALISK_WIRE");
	//울트라 move
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 9; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/Zerg/Ultralisk/Move/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Ultralisk_Move%d_Anim%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//울트라 portrait
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Ultralisk/Portrait/%d.png", r);
		wsprintf(szKey, L"Ultralisk_Portrait_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//울트라 attack
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 6; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/Zerg/Ultralisk/Attack/Attack%d/%d.png", r, c);
			wsprintf(szKey, L"Ultralisk_Attack%d_Anim%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Ultralisk/Wire/0.png", L"ULTRALISK_WIRE");
}

void CStage::LoadProtossResource()
{
	//프로토스 메인 메뉴 
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/MainUI/MainUI.png", L"PROTOSS_MAIN_UI");
	//쉴드 rect
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Bar/ShieldBar/Shield_Rect.png", L"Shield_Rect");
	//Mp Rect
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Bar/MPBar/MpRect.png", L"Mp_Rect");
	//프로브 커맨드 슬롯 버튼
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/LBuild/LBuild_0.png", L"LBuild_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/LBuild/LBuild_1.png", L"LBuild_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/HBuild/HBuild_0.png", L"ABuild_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/HBuild/HBuild_1.png", L"ABuild_1");
	//LBuilding Button
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Nexus/Nexus_0.png", L"Nexus_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Nexus/Nexus_1.png", L"Nexus_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Pylon/Pylon_0.png", L"Pylon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Pylon/Pylon_1.png", L"Pylon_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Gateway/Gateway_0.png", L"Gateway_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Gateway/Gateway_1.png", L"Gateway_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Assimilator/Assimilator_0.png", L"Assimilator_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Assimilator/Assimilator_1.png", L"Assimilator_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Gateway/Gateway_0.png", L"Gateway_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Gateway/Gateway_1.png", L"Gateway_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Forge/Forge_0.png", L"Forge_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Forge/Forge_2.png", L"Forge_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Photon Cannon/Photon Cannon_0.png", L"Photon_Cannon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Photon Cannon/Photon Cannon_2.png", L"Photon_Cannon_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Photon Cannon/Photon Cannon_2.png", L"Photon_Cannon_2");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Cybernetics Core/Cybernetics Core_0.png", L"Cybernetics_Core_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Cybernetics Core/Cybernetics Core_2.png", L"Cybernetics_Core_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Cybernetics Core/Cybernetics Core_2.png", L"Cybernetics_Core_2");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Shield Battery/Shield Battery_0.png", L"Shield_Battery_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Shield Battery/Shield Battery_2.png", L"Shield_Battery_1");
	//RBuilding Button
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Robotics Facility/Robotics Facility_0.png", L"Robotics_Facility_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Robotics Facility/Robotics Facility_2.png", L"Robotics_Facility_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Stargate/Stargate_0.png", L"Stargate_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Stargate/Stargate_2.png", L"Stargate_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Citadel of Adun/Citadel of Adun_0.png", L"CitadelOfAdun_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Citadel of Adun/Citadel of Adun_2.png", L"CitadelOfAdun_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Robotics Support Bay/Robotics Support Bay_0.png", L"RoboticsSupportBay_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Robotics Support Bay/Robotics Support Bay_2.png", L"RoboticsSupportBay_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Fleet Beacon/Fleet Beacon_0.png", L"Fleet_Beacon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Fleet Beacon/Fleet Beacon_2.png", L"Fleet_Beacon_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Tempar Archives/Tempar Archives_0.png", L"Templar_Archives_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Tempar Archives/Tempar Archives_2.png", L"Templar_Archives_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Observatory/Observatory_0.png", L"Observatory_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Observatory/Observatory_2.png", L"Observatory_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter Tribunal/Arbiter Tribunal_0.png", L"Arbiter_Tribunal_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter Tribunal/Arbiter Tribunal_2.png", L"Arbiter_Tribunal_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Cancel/Cancel_0.png", L"Cancle_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Cancel/Cancel_1.png", L"Cancle_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter/Arbiter_0.png", L"Arbiter_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter/Arbiter_1.png", L"Arbiter_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter/Arbiter_2.png", L"Arbiter_2");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Archon/Archon_0.png", L"Archon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Archon/Archon_1.png", L"Archon_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Archon/Archon_2.png", L"Archon_2");


	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Move/move_0.png", L"Move_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Move/move_1.png", L"Move_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Stop/stop_0.png", L"Stop_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Stop/stop_1.png", L"Stop_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Hold/Hold_0.png", L"Hold_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Hold/Hold_1.png", L"Hold_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Patrol/patrol_0.png", L"Patrol_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Patrol/patrol_1.png", L"Patrol_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Att/att_0.png", L"Attack_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Att/att_1.png", L"Attack_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Rally/Rally_0.png", L"Rally_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Rally/Rally_1.png", L"Rally_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Storm/Storm_0.png", L"Storm_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Storm/Storm_2.png", L"Storm_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Archon/Archon_0.png", L"Wrap_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Archon/Archon_2.png", L"Wrap_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Hal/hal0.png", L"Hallucination_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Hal/hal1.png", L"Hallucination_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Interceptor/Interceptor_0.png", L"Interceptor_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Interceptor/Interceptor_1.png", L"Interceptor_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Recall/Recall_0.png", L"Recall_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Recall/Recall_2.png", L"Recall_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Ice/Ice_0.png", L"Ice_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Ice/Ice_1.png", L"Ice_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Ice/Ice_0.png", L"Ice_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Ice/Ice_1.png", L"Ice_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Boarding/Boarding_0.png", L"Loading_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Boarding/Boarding_1.png", L"Loading_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Landing/Landing_0.png", L"Landing_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Landing/Landing_1.png", L"Landing_1");

	//질럿 돌진
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/ZealotSpeedUP/ZealotSpeedUP_0.png", L"Rush_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/ZealotSpeedUP/ZealotSpeedUP_1.png", L"Rush_1");
	//드라군
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Dragoon/Dragoon_0.png", L"Dragoon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Dragoon/Dragoon_2.png", L"Dragoon_1");
	//드라군 점멸(RangeUP)
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/RangeUP/RangeUP_0.png", L"Blink_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/RangeUP/RangeUP_1.png", L"Blink_1");
	//유닛 버튼
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Probe/Probe_0.png", L"Probe_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Probe/Probe_1.png", L"Probe_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Zealot/Zealot_0.png", L"Zealot_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Zealot/Zealot_1.png", L"Zealot_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Dragoon/Dragoon_0.png", L"Dragoon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Dragoon/Dragoon_2.png", L"Dragoon_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/HighTemplar/HighTemplar_0.png", L"HighTemplar_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/HighTemplar/HighTemplar_2.png", L"HighTemplar_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/DarkTemplar/DarkTemplar_0.png", L"DarkTemplar_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/DarkTemplar/DarkTemplar_2.png", L"DarkTemplar_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Observer/Observer_0.png", L"Observer_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Observer/Observer_2.png", L"Observer_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Shuttle/Shuttle_0.png", L"Shuttle_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Shuttle/Shuttle_2.png", L"Shuttle_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Reaver/Reaver_0.png", L"Reaver_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Reaver/Reaver_2.png", L"Reaver_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Corsair/Corsair_0.png", L"Corsair_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Corsair/Corsair_2.png", L"Corsair_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Scout/Scout_0.png", L"Scout_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Scout/Scout_2.png", L"Scout_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Carrier/Carrier_0.png", L"Carrier_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Carrier/Carrier_2.png", L"Carrier_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter/Arbiter_0.png", L"Arbiter_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter/Arbiter_2.png", L"Arbiter_1");

	//다크 아칸
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/MindControl/MindControl_0.png", L"MindControl_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/MindControl/MindControl_2.png", L"MindControl_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Maelstrom/Maelstrom0.png", L"Maelstrom_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Maelstrom/Maelstrom0.png", L"Maelstrom_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Feedback/Feedback0.png", L"Feedback_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Feedback/Feedback0.png", L"Feedback_1");

	//프로토스 건물
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Nexus/Img/Nexus_0.png", L"Nexus");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Nexus/BigWire_06.png", L"Nexus_BigWire");

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Protoss/Building/Pylon/Img/Pylon_0_bmp.bmp", L"Pylon");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Pylon/BigWire_06.png", L"Pylon_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Pylon/Range/Pylon_0_range.png", L"Pylon_Range");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Assimilator/Img/Assimilator_0.png", L"Assimilator");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Gas/BigWire_06.png", L"Assimilator_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Gateway/Img/Gateway_0.png", L"Gateway");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Gateway/BigWire_06.png", L"Gateway_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Forge/Img/Forge_0.png", L"Forge");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Forge/BigWire_06.png", L"Forge_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Photon Cannon/Img/Photon Cannon_3.png", L"Photon_Cannon");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Cybernetics Core/Img/Cybernetics Core_0_bmp.bmp", L"Cybernetics_Core");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Core/BigWire_06.png", L"Core_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Shield Battery/Img/Shield Battery_0.png", L"Shield_Battery");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Battery/BigWire_06.png", L"ShieldBattery_BigWire");

	//ABuilding
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Robotics Facility/Img/Robotics Facility_0.png", L"Robotics_Facility");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Robotics/BigWire_06.png", L"Robotics_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Stargate/Img/Stargate_0.png", L"Stargate");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Stargate/BigWire_06.png", L"Stargate_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Citadel of Adun/Img/Citadel of Adun_0.png", L"CitadelOfAdun");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Adun/BigWire_06.png", L"Adun_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Robotics Support Bay/Img/Robotics Support Bay_0.png", L"Robotics_Support_Bay");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/RoboticsSupportBay/BigWire_06.png", L"RoboticsSupportBay_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Fleet Beacon/Img/Fleet Beacon_0.png", L"Fleet_Beacon");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/FleetBeacon/BigWire_06.png", L"FleetBeacon_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Tempar Archives/Img/Tempar Archives_0.png", L"Templar_Archives");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/TemplarArchive/BigWire_06.png", L"TemplarArchive_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Observatory/Img/Observatory_0.png", L"Observatory");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Observatory/BigWire_06.png", L"Observatory_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Arbiter Tribunal/Img/Arbiter Tribunal_0.png", L"Arbiter_Tribunal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/ArbitorTribunal/BigWire_06.png", L"ArbitorTribunal_BigWire");

	//프로토스 건물 파괴 이펙트
	for (int r = 0; r < 14; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Explosion_L/tbangx%d.png", r);
		wsprintf(szKey, L"Protoss_Building_Destroy_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}

	//프로토스 유닛 건물 초상화
	for (int r = 5; r < 75; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/UI/Portraits/Aldaris/Portraits_%d.png", r);
		wsprintf(szKey, L"Protoss_Portraits_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//빌드 템플릿
	for (int r = 0; r < 14; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Warp/warp%d.png", r);
		wsprintf(szKey, L"Protoss_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//넥서스 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Nexus/BuildTime/nexus%d.png", r);
		wsprintf(szKey, L"Nexus_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}

	//파일런 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Pylon/BuildTime/pylon%d.png", r);
		wsprintf(szKey, L"Pylon_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Assimilator 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Assimilator/BuildTime/assim%d.png", r);
		wsprintf(szKey, L"Assimilator_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Gateway 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Gateway/BuildTime/gateway%d.png", r);
		wsprintf(szKey, L"Gateway_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Forge 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Forge/BuildTime/forge%d.png", r);
		wsprintf(szKey, L"Forge_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Cybernetics_Core 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Cybernetics Core/BuildTime/core%d.png", r);
		wsprintf(szKey, L"Cybernetics_Core_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Shield Battery 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Shield Battery/BuildTime/battery%d.png", r);
		wsprintf(szKey, L"ShieldBattery_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Shield Battery 치유 이펙트
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Hallucination/hallucination_%d.png", r);
		wsprintf(szKey, L"ShieldBattery_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Robotics 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Robotics Facility/BuildTime/robot%d.png", r);
		wsprintf(szKey, L"Robotics_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Stargate 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Stargate/BuildTime/stgate%d.png", r);
		wsprintf(szKey, L"Stargate_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//CitadelOfAdun 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Citadel of Adun/BuildTime/adun%d.png", r);
		wsprintf(szKey, L"CitadelOfAdun_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Robotics Support Bay 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Robotics Support Bay/BuildTime/robotsp%d.png", r);
		wsprintf(szKey, L"RoboticsSupportBay_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Fleet Beacon 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Fleet Beacon/BuildTime/beacon%d.png", r);
		wsprintf(szKey, L"FleetBeacon_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Templar Archives 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Tempar Archives/BuildTime/templar%d.png", r);
		wsprintf(szKey, L"TemplarArchives_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Observatory 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Observatory/BuildTime/observ%d.png", r);
		wsprintf(szKey, L"Observatory_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Arbiter Tribunal 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Arbiter Tribunal/BuildTime/arbiter%d.png", r);
		wsprintf(szKey, L"ArbiterTribunal_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//프로브
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Probe/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Probe_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//프로브 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Probe/BigWire_06.png", L"Probe_BigWire");
	//프로브 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Probe/grpwire06.png", L"Probe_SmallWire");
	//셔틀
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Shuttle/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Shuttle_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//셔틀 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Shuttle/BigWire_06.png", L"Shuttle_BigWire");
	//셔틀 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Shuttle/grpwire86.png", L"Shuttle_SmallWire");
	//질럿
	for (int frame = 0; frame < 7; ++frame)
	{
		for (int dir = 0; dir < 17; ++dir)
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Zealot/Move/zealot%d.png", index);
			wsprintf(szKey, L"Zealot_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//질럿 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Zealot/BigWire_06.png", L"Zealot_BigWire");
	//질럿 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Zealot/grpwire16.png", L"Zealot_SmallWire");
	//질럿 attack
	for (int frame = 0; frame < 7; ++frame)
	{
		for (int dir = 0; dir < 17; ++dir)
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Zealot/Att/zealot%d.png", index);
			wsprintf(szKey, L"Zealot_Attack%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//아칸
	for (int frame = 0; frame < 4; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Archon/Normal/normal_%d.png", index);
			wsprintf(szKey, L"Archon_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//아칸 몸체
	for (int r = 0; r < 10; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Archon/Body/body_%d.png", r);
		wsprintf(szKey, L"Archon_Body_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//아칸 Attack
	for (int frame = 0; frame < 10; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Archon/Att/att_%d.png", index);
			wsprintf(szKey, L"Archon_Attack%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//아칸 Create
	for (int r = 0; r < 10; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Archon/Create/create_%d.png", r);
		wsprintf(szKey, L"Archon_Create_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//아칸 Shockball
	for (int r = 0; r < 6; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Shockball/Shockball_%d.png", r);
		wsprintf(szKey, L"Archon_Shockball_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Dark Archon/Normal/normal_%d.png", r);
		wsprintf(szKey, L"DarkArchon_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/DarkArchon/BigWire_06.png", L"DarkArchon_BigWire");
	//다크 아칸 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/DarkArchon/grpwire66.png", L"DarkArchon_SmallWire");
	//다크 아칸 몸체
	for (int r = 0; r < 10; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Dark Archon/Body/body_%d.png", r);
		wsprintf(szKey, L"DarkArchon_Body_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 아우라
	for (int r = 0; r < 15; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Dark Archon/Att/att_%d.png", r);
		wsprintf(szKey, L"DarkArchon_Aura_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 Attack
	for (int frame = 0; frame < 10; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Dark Archon/Att/Att/att_%d.png", index);
			wsprintf(szKey, L"DarkArchon_Attack%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//다크 아칸 공격 이펙트
	for (int frame = 0; frame < 3; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Effect/Shockwave_red/Shockwave/shock_%d.png", index);
			//wsprintf(szPath, L"../Image/Protoss/Effect/Shockwave/shock_%d.png", index);
			wsprintf(szKey, L"DarkArchon_Shockwave%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//다크 아칸 Shock_Ball
	for (int r = 0; r < 6; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Shockball_red/Shockball/Shockball_%d.png", r);
		wsprintf(szKey, L"DarkArchon_Shockball_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 Create
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Dark Archon/Create/create_%d.png", r);
		wsprintf(szKey, L"DarkArchon_Create_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 마인드 컨트롤 이펙트
	for (int r = 0; r < 20; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/MindControl/mindcontrol_%d.png", r);
		wsprintf(szKey, L"MindControl_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 DIE
	for (int r = 0; r < 15; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/DarkArchonDie/die_%d.png", r);
		wsprintf(szKey, L"DArchon_Die_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크템플러 move
	for (int frame = 0; frame < 8; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/DarkTemplar/Move/move_%d.png", index);
			wsprintf(szKey, L"DarkTemplar_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//다크 템플러 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/DarkTemplar/BigWire_06.png", L"DarkTemplar_BigWire");
	//다크 템플러 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/DarkTemplar/grpwire56.png", L"DarkTemplar_SmallWire");
	//다크 템플러 DIE
	for (int r = 0; r < 7; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/DarkTemplarDie/die_%d.png", r);
		wsprintf(szKey, L"DarkTemplar_Die_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크템플러 attack
	for (int frame = 0; frame < 10; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/DarkTemplar/Att/att_%d.png", index);
			wsprintf(szKey, L"DarkTemplar_Attack%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//드라군
	for (int frame = 0; frame < 8; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Dragoon/Move/move_%d.png", index);
			wsprintf(szKey, L"Dragoon_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//다크 템플러 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Dragoon/BigWire_06.png", L"Dragoon_BigWire");
	//다크 템플러 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Dragoon/grpwire26.png", L"Dragoon_SmallWire");
	//드라군 att
	for (int r = 0; r < 8; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Dragoon/Att/att_%d.png", r);
		wsprintf(szKey, L"Dragoon_Attack_Anim%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//드라군 ball
	for (int r = 0; r < 5; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Ball/Ball_%d.png", r);
		wsprintf(szKey, L"Dragoon_Ball_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//드라군 ball bmp
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Protoss/Effect/Ball_bmp/Ball/Ball_0.bmp", L"Dragoon_Ball");

	//드라군 ball ex
	for (int r = 0; r < 14; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/BallEx/ballex_%d.png", r);
		wsprintf(szKey, L"Dragoon_BallEx_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//아비터
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Arbiter/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Arbiter_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//아비터 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Arbiter/BigWire_06.png", L"Arbiter_BigWire");
	//아비터 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Arbiter/grpwire146.png", L"Arbiter_SmallWire");
	//아비터 Recall
	for (int r = 0; r < 11; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Recall/recall%d.png", r);
		wsprintf(szKey, L"Arbiter_Recall_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//캐리어
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Carrier/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Carrier_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//캐리어 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Carrier/BigWire_06.png", L"Carrier_BigWire");
	//캐리어 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Carrier/grpwire116.png", L"Carrier_SmallWire");
	//인터셉터
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Interceptor/Normal/normal_%d.png", r);
		wsprintf(szKey, L"Interceptor_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//인터셉터 attack
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Interceptor/Att/att_%d.png", r);
		wsprintf(szKey, L"Interceptor_Attack_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//인터셉터 bullet
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Intercep_Bullet/bullet_%d.png", r);
		wsprintf(szKey, L"Interceptor_Bullet_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//커세어
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Corsair/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Corsair_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//커세어 Att
	for (int frame = 0; frame < 4; ++frame)  // 동작 프레임 0~4
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Corsair/Att/att_%d.png", index);
			wsprintf(szKey, L"Corsair_Attack%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//커세어 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Corsair/BigWire_06.png", L"Corsair_BigWire");
	//커세어 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Corsair/grpwire136.png", L"Corsair_SmallWire");
	//하이템플러
	for (int frame = 0; frame < 4; ++frame)  // 동작 프레임 0~4
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/HighTemplar/Move/move_%d.png", index);
			wsprintf(szKey, L"HighTemplar_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//하이 템플러 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/HighTemplar/BigWire_06.png", L"HighTemplar_BigWire");
	//하이 템플러 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/HighTemplar/grpwire36.png", L"HighTemplar_SmallWire");
	//하이템플러 그림자
	for (int frame = 0; frame < 3; ++frame)  // 동작 프레임 0~4
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/HighTemplar/Shad/shad_%d.png", index);
			wsprintf(szKey, L"HighTemplar_Shad%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//하이템플러 스톰
	for (int r = 0; r < 14; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Storm/Storm_%d.png", r);
		wsprintf(szKey, L"Storm_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//하이템플러 attack
	for (int frame = 0; frame < 5; ++frame)  // 동작 프레임 0~4
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/HighTemplar/Att/att_%d.png", index);
			wsprintf(szKey, L"HighTemplar_Attack%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//하이템플러 DIE
	for (int r = 0; r < 7; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/HighTemplarDie/die_%d.png", r);
		wsprintf(szKey, L"HighTemplar_Die_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//리버
	for (int frame = 0; frame < 9; ++frame)  // 동작 프레임 0~4
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Reaver/Move/move_%d.png", index);
			wsprintf(szKey, L"Reavor_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//리버 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Reaver/BigWire_06.png", L"Reaver_BigWire");
	//리버 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Reaver/grpwire76.png", L"Reaver_SmallWire");
	//스카웃
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Scout/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Scout_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//스카웃 Attack
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Scout/Att/att_%d.png", r);
		wsprintf(szKey, L"Scout_Attack_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//스카웃 ground effect
	for (int r = 0; r < 8; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Scout_Ground/ground_%d.png", r);
		wsprintf(szKey, L"Scout_GroundEffect_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//스카웃 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Scout/BigWire_06.png", L"Scout_BigWire");
	//스카웃 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Scout/grpwire106.png", L"Scout_SmallWire");
	//옵저버
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Observer/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Observer_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//옵저버 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Observer/BigWire_06.png", L"Observer_BigWire");
	//옵저버 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Observer/grpwire96.png", L"Observer_SmallWire");
}

void CStage::SpawnTerranBase()
{
	//마린 3마리 생성
	//CObj* pMarin = CAbstractFactory<CMarine>::Create(300.f, 300.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMarin);
	//pMarin = CAbstractFactory<CMarine>::Create(400.f, 300.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMarin);
	//pMarin = CAbstractFactory<CMarine>::Create(500.f, 300.f);
	//CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMarin);
	//SCV 생성
	CObj* pSCV = CAbstractFactory<CSCV>::Create(240.f, 300.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	pSCV = CAbstractFactory<CSCV>::Create(280.f, 300.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	pSCV = CAbstractFactory<CSCV>::Create(300.f, 300.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	pSCV = CAbstractFactory<CSCV>::Create(320.f, 300.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	//커맨드 센터 생성
	CObj* pCC = CAbstractFactory<CCommandCenter>::Create(292.f, 236.f);
	CCommandCenter* pComC = dynamic_cast<CCommandCenter*>(pCC);
	//커맨드 센터 초기화
	pComC->SetGhost(false); pComC->SetState(eBuildingState::CONSTRUCT);
	pComC->SetHP(pComC->Get_MaxHP());
	pComC->AppplyOccupy();
	CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pCC);
}