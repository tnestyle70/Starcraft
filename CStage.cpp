#include "pch.h"
#include "CStage.h"
#include "CBmpMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CScrollMgr.h"
#include "CTimeMgr.h"
#include "CInputMgr.h"
#include "CSceneMgr.h"
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

CStage::CStage()
{
}

CStage::~CStage()
{
}

void CStage::Initialize()
{
	LoadImageResource();
	CreateObject();
	//리소스 매니저 - 나중에 지우기
	CResourceMgr::Get_Instance()->Reset();
	//Tile 정보 로드
	CTileMgr::Get_Instance()->Load_Tile();
	//Stage 진입시 마우스 클램핑
	ClampMouse(g_hWnd);
	//NavGrid 정보 로드
	CNavMgr::Get_Instance()->BuildFromTile();
	//UIMgr 초기화 (여기서 MainUI.bmp 로드 + 알파 비트맵 생성)
	CUIMgr::Get_Instance()->Initialize();
	//FogMgr 초기화
	CFogMgr::Get_Instance()->Initialize();
	//CursorMgr 초기화
	CCursorMgr::Get_Instance()->Initialize();
	//미네랄 생성
	CreateResource();
}

int CStage::Update()
{
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

	if (CInputMgr::Get_Instance()->KeyDown(RIGHT_MOUSE))
	{
		//커서 상태 변경
		CCursorMgr::Get_Instance()->SetClickEffect();

		Vec2 worldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
		//마우스 주변에 오브젝트가 존재할 경우 ISSUEATTACK
		CObj* pClickedObj = FindObjectAtPosition(worldMouse);
		if (pClickedObj)
		{
			CCommandMgr::Get_Instance()->IssueAttack(pClickedObj);
		}
		else //존재하지 않을 경우 이동
		{
			CCommandMgr::Get_Instance()->IssueAttackMove(worldMouse);
		}
		CCommandMgr::Get_Instance()->IssueMove(worldMouse);
	}
	//A키 + Left Mouse 클릭으로 A땅 공격 구현
	if (CInputMgr::Get_Instance()->KeyDown(A_KEY))
	{
		m_bAttackMoveMode = true;
	}
	if (m_bAttackMoveMode && CInputMgr::Get_Instance()->KeyDown(LEFT_MOUSE))
	{
		Vec2 worldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
		CObj* pClickedObj = FindObjectAtPosition(worldMouse);
		if (pClickedObj)
			CCommandMgr::Get_Instance()->IssueAttack(pClickedObj);
		else
			CCommandMgr::Get_Instance()->IssueAttackMove(worldMouse);
		m_bAttackMoveMode = false;
	}

	//강제 종료
	if (CInputMgr::Get_Instance()->KeyPress(ESCAPE0))
	{
		ClipCursor(nullptr);
		//CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_MENU);
		DestroyWindow(g_hWnd);
	}
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
	float searchRadius = 20.f; //클릭 인식 반경
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

void CStage::LoadImageResource()
{
	//스테이지 이미지
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MyFolder/Map/FightSpirit.bmp", L"Stage");
	//마린 스프라이트 시트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Marine/Marine.bmp", L"Marine");
	//배틀크루저 스프라이트 시트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/BattleCruiser/BattleCruiser.bmp", L"BattleCruiser");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/BattleCrusor/BattleAttack.bmp", L"BCBullet");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/BattleCrusor/Yamato.bmp", L"BCYamato");
	//탱크 스프라이트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Tank/TankBody.bmp", L"Tank_Body");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Tank/TankHead.bmp", L"Tank_Head");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/TankAtt/SiegeTankHit.bmp", L"SiegeTankHit");
	//SCV
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/SCV/SCV.bmp", L"SCV");
	//Medic
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Medic/Medic.bmp", L"Medic");
	//Vulture
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Vulture/Vulture.bmp", L"Vulture");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Bullet/VultureAtt/grenade0.png", L"VBullet");
	//Goliath
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Goliath/GoliathBody.bmp", L"Goliath_Body");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Goliath/GoliathHead.bmp", L"Goliath_Head");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Bullet/MissileAtt/missile0.png", L"GBullet");
	//커맨드 센터
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/CommandCenter/CommandCenter.bmp", L"CommandCenter");
	//블루 커맨드 센터 생성
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/trash/Enemy/Blue_CommandCenter.bmp", L"Blue_CommandCenter");
	//배럭, 스타포트, 보급고, 군수공장, 정제소
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Barrack/Barrack.bmp", L"Barracks");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Starport/Starport.bmp", L"Starport");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/SupplyDepot/SupplyDepot.bmp", L"SupplyDepot");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Factory/Factory.bmp", L"Factory");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Refinery/Refinery4.bmp", L"Refinery");
	//미네랄
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Resource/Mineral0.bmp", L"Mineral");
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
	//블루 커맨드 센터 생성
	CObj* pBlueCC = CAbstractFactory<CCommandCenter>::Create(600.f, 600.f);
	CCommandCenter* pBlueComC = dynamic_cast<CCommandCenter*>(pBlueCC);
	//커맨드 센터 초기화
	pBlueComC->SetGhost(false); pBlueComC->SetState(eBuildingState::CONSTRUCT);
	pBlueComC->SetHP(pBlueComC->Get_MaxHP()); pBlueComC->AppplyOccupy();
	pBlueComC->SetFrameKey(L"Blue_CommandCenter");
	CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pBlueComC);
}
