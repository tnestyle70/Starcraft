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
#include "CUIMgr.h"

CStage::CStage()
{
}

CStage::~CStage()
{
}

void CStage::Initialize()
{
	//스테이지 이미지
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MyFolder/Map/FightSpirit.bmp", L"Stage");
	//Stage 진입시 마우스 클램핑
	ClampMouse(g_hWnd);
	//Tile 정보 로드
	CTileMgr::Get_Instance()->Load_Tile();
	//NavGrid 정보 로드
	CNavMgr::Get_Instance()->BuildFromTile();

	//마린 스프라이트 시트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Marine/Marine.bmp", L"Marine");
	//마린 3마리 생성
	CObj* pMarin = CAbstractFactory<CMarine>::Create(300.f, 300.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMarin);
	pMarin = CAbstractFactory<CMarine>::Create(400.f, 300.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMarin);
	pMarin = CAbstractFactory<CMarine>::Create(500.f, 300.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMarin);
	//배틀크루저 스프라이트 시트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/BattleCruiser/BattleCruiser.bmp", L"BattleCruiser");
	//배틀크루저 생성
	CObj* pBattleCruiser = CAbstractFactory<CBattleCruiser>::Create(600.f, 400.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pBattleCruiser);
	pBattleCruiser = CAbstractFactory<CBattleCruiser>::Create(500.f, 400.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pBattleCruiser);
	pBattleCruiser = CAbstractFactory<CBattleCruiser>::Create(700.f, 400.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pBattleCruiser);
	//탱크 스프라이트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Tank/TankBody.bmp", L"Tank");
	CObj* pTank = CAbstractFactory<CTank>::Create(200.f, 500.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pTank);
	pTank = CAbstractFactory<CTank>::Create(300.f, 500.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pTank);
	pTank = CAbstractFactory<CTank>::Create(100.f, 500.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pTank);
	//SCV
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/SCV/SCV.bmp", L"SCV");
	CObj* pSCV = CAbstractFactory<CSCV>::Create(100.f, 100.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	pSCV = CAbstractFactory<CSCV>::Create(200.f, 100.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	pSCV = CAbstractFactory<CSCV>::Create(300.f, 100.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	//Medic
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Medic/Medic.bmp", L"Medic");
	CObj* pMedic = CAbstractFactory<CMedic>::Create(400.f, 200.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMedic);

	//커맨드 센터
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/CommandCenter/CommandCenter4.bmp", L"CommandCenter");
	CObj* pCommandCenter = CAbstractFactory<CCommandCenter>::Create(200.f, 200.f);
	CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pCommandCenter);
	//UIMgr 초기화 (여기서 MainUI.bmp 로드 + 알파 비트맵 생성)
	CUIMgr::Get_Instance()->Initialize();
}

int CStage::Update()
{
	CSelectionMgr::Get_Instance()->Update();

	CObjMgr::Get_Instance()->Update();
	
	//CUIMgr::Get_Instance()->Update();

	return 0;
}

void CStage::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();

	ClampMouse(g_hWnd);

	float fDT = CTimeMgr::Get_Instance()->GetDT();
	CScrollMgr::Get_Instance()->Update_EdgeScroll(fDT);

	if (CInputMgr::Get_Instance()->KeyDown(RIGHT_MOUSE))
	{
		Vec2 vWMouse = CInputMgr::Get_Instance()->GetWorldMouse();
		CCommandMgr::Get_Instance()->IssueMove(vWMouse);
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

	// 2. 오브젝트 렌더
	CObjMgr::Get_Instance()->Render(hDC);

	// 3. UI 렌더 (가장 위)
	CUIMgr::Get_Instance()->Render(hDC);
}

void CStage::Release()
{
	CObjMgr::Get_Instance()->Delete_Obj(OBJ_UI);
	CUIMgr::Get_Instance()->Release();
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
