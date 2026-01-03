#include "pch.h"
#include "CMenuRace.h"
#include "CButton.h"
#include "CAnimButton.h"
#include "CBmpMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CCursorMgr.h"

CMenuRace::CMenuRace()
{
}

CMenuRace::~CMenuRace()
{
}

void CMenuRace::Initialize()
{
	//커서 초기화
	CCursorMgr::Get_Instance()->Initialize();
	//배경 이미지 
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu2/BackGround.png", L"Menu");
	InitializeAnim();
	InitializeButton();
}

int CMenuRace::Update()
{
	CObjMgr::Get_Instance()->Update();

	CCursorMgr::Get_Instance()->Update();

	return 0;
}

void CMenuRace::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();
}

void CMenuRace::Render(HDC hDC)
{
	CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(L"Menu");
	if (pPng)
	{
		pPng->Render_Alpha_Pure(hDC, 0, 0, 800, 600);
	}
	CObjMgr::Get_Instance()->Render(hDC);
	// 커서 
	CCursorMgr::Get_Instance()->Render(hDC);
}

void CMenuRace::Release()
{
	CObjMgr::Get_Instance()->Delete_Obj(OBJ_UI);
}

void CMenuRace::InitializeAnim()
{
	//종족 애니메이션 생성
	 
	//테란
	CObj* pAnimButton = CAbstractFactory<CAnimButton>::Create(200.f, 300.f);
	pAnimButton->Initialize();
	CAnimButton* pAnimBtn = dynamic_cast<CAnimButton*>(pAnimButton);
	for (int i = 0; i <= 77; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/Terran/%d.png", i);
		wsprintf(szKey, L"Terran_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i <= 30; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/TerranMouseOn/%d.png", i);
		wsprintf(szKey, L"Terran_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	if (pAnimBtn)
	{
		pAnimBtn->Set_NormalImage(L"Terran_Normal_Anim_0");
		pAnimBtn->Set_NormalAnimation(L"Terran_Normal_Anim_%d", 77);
		pAnimBtn->Set_HoverAnimation(L"Terran_Hover_Anim_%d", 30);
		pAnimBtn->Set_ButtonType(L"Start");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pAnimButton);
	
	//프로토스
	pAnimButton = CAbstractFactory<CAnimButton>::Create(450.f, 200.f);
	pAnimButton->Initialize();
	pAnimBtn = dynamic_cast<CAnimButton*>(pAnimButton);
	for (int i = 0; i <= 69; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/Protoss/%d.png", i);
		wsprintf(szKey, L"Protoss_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i <= 30; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/ProtossMouseOn/%d.png", i);
		wsprintf(szKey, L"Protoss_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	if (pAnimBtn)
	{
		pAnimBtn->Set_NormalImage(L"Protoss_Normal_Anim_0");
		pAnimBtn->Set_NormalAnimation(L"Protoss_Normal_Anim_%d", 69);
		pAnimBtn->Set_HoverAnimation(L"Protoss_Hover_Anim_%d", 30);
		pAnimBtn->Set_ButtonType(L"Start");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pAnimButton);

	//저그
	pAnimButton = CAbstractFactory<CAnimButton>::Create(500.f, 500.f);
	pAnimButton->Initialize();
	pAnimBtn = dynamic_cast<CAnimButton*>(pAnimButton);
	for (int i = 0; i <= 69; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/Zerg/%d.png", i);
		wsprintf(szKey, L"Zerg_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i <= 19; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/ZergMouseOn/%d.png", i);
		wsprintf(szKey, L"Zerg_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	if (pAnimBtn)
	{
		pAnimBtn->Set_NormalImage(L"Zerg_Normal_Anim_0");
		pAnimBtn->Set_NormalAnimation(L"Zerg_Normal_Anim_%d", 69);
		pAnimBtn->Set_HoverAnimation(L"Zerg_Hover_Anim_%d", 19);
		pAnimBtn->Set_ButtonType(L"Start");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pAnimButton);
}

void CMenuRace::InitializeButton()
{
	//PNG 버튼 이미지 로딩
	/*
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/single_button0.png", L"Btn_Single_Normal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/single_button1.png", L"Btn_Single_Hover");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/editor_button0.png", L"Btn_Editor_Normal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/editor_button1.png", L"Btn_Editor_Hover");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/exit_button0.png", L"Btn_Exit_Normal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/exit_button1.png", L"Btn_Exit_Hover");
	//SinglePlay 이후 버튼
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Logo/SelectMenu1.png", L"Select_Menu");

	//Single 버튼
	CObj* pButton = CAbstractFactory<CButton>::Create(200.f, 220.f);
	pButton->Initialize();
	CButton* pBtn = dynamic_cast<CButton*>(pButton);
	if (pBtn)
	{
		pBtn->Set_PngImages(L"Btn_Single_Normal", L"Btn_Single_Hover");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pButton);
	//Editor 버튼
	pButton = CAbstractFactory<CButton>::Create(600.f, 250.f);
	pButton->Initialize();
	pBtn = dynamic_cast<CButton*>(pButton);
	if (pBtn)
	{
		pBtn->Set_PngImages(L"Btn_Editor_Normal", L"Btn_Editor_Hover");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pButton);
	//Exit 버튼
	pButton = CAbstractFactory<CButton>::Create(550.f, 400.f);
	pButton->Initialize();
	pBtn = dynamic_cast<CButton*>(pButton);
	if (pBtn)
	{
		pBtn->Set_PngImages(L"Btn_Exit_Normal", L"Btn_Exit_Hover");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pButton);
	*/
}
