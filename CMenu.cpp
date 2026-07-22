#include "pch.h"
#include "CMenu.h"
#include "CButton.h"
#include "CAnimButton.h"
#include "CBmpMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CCursorMgr.h"
#include "CSoundMgr.h"
#include "CTimeMgr.h"
#include "CResourceLoader.h"

CMenu::CMenu()
{
}

CMenu::~CMenu()
{
}

void CMenu::Initialize()
{
	//커서 초기화
	CCursorMgr::Get_Instance()->Initialize();
	//배경 이미지 
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MyFolder/MenuBack.bmp", L"Menu");
	//로고 이미지
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Logo/Logo.bmp", L"Logo");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Logo/Logo_Blue.png", L"LogoBlue");

	//사운드 재생
	CSoundMgr::Get_Instance()->PlayBGM(L"BGM/Title.wav", 2.f);

	LoadResource();
	InitializeAnim();
	InitializeButton();
}

int CMenu::Update()
{
	if (m_bLogo)
	{
		m_fLogoTime += CTimeMgr::Get_Instance()->GetDT();
		if (m_fLogoTime >= m_fLogoDuration)
		{
			m_bLogo = false;
		}
		return NOEVENT;
	}	
	CObjMgr::Get_Instance()->Update();

	CCursorMgr::Get_Instance()->Update();

	return 0;
}

void CMenu::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();
}

void CMenu::Render(HDC hDC)
{
	//CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(L"LogoBlue");
	//if (pPng)
	//{
	//	int width = pPng->Get_Width();
	//	int height = pPng->Get_Height();
	//	pPng->Render_Alpha(hDC, width, height, false, false);
	//}
	if (m_bLogo)
	{
		HDC	hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Logo");

		BitBlt(hDC,		// 복사 받을 DC
			0,
			0,
			WINCX,
			WINCY,
			hMemDC,		// 복사할 이미지 DC
			0,			// 이미지의 LEFT
			0,			// 이미지의 TOP
			SRCCOPY);	// 복사 방식	

		return;
	}

	HDC	hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Menu");

	BitBlt(hDC,		// 복사 받을 DC
		0,
		0,
		WINCX,
		WINCY,
		hMemDC,		// 복사할 이미지 DC
		0,			// 이미지의 LEFT
		0,			// 이미지의 TOP
		SRCCOPY);	// 복사 방식

	CObjMgr::Get_Instance()->Render(hDC);
	// 커서 
	CCursorMgr::Get_Instance()->Render(hDC);
}

void CMenu::Release()
{
	CObjMgr::Get_Instance()->Delete_Obj(OBJ_UI);
}

void CMenu::LoadResource()
{
	//싱글, 에디터, Exit 버튼
	//PNG 버튼 이미지 로딩
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/single_button0.png", L"Btn_Single_Normal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/single_button1.png", L"Btn_Single_Hover");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/multiplayer_button.png", L"Btn_Multiplayer_Normal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/editor_button0.png", L"Btn_Editor_Normal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/editor_button1.png", L"Btn_Editor_Hover");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/exit_button0.png", L"Btn_Exit_Normal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/exit_button1.png", L"Btn_Exit_Hover");
	//SinglePlay 이후 버튼
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Logo/SelectMenu1.png", L"Select_Menu");

	//싱글 애니메이션
	for (int i = 0; i < 35; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/SinglePlay/Animation/single%d.png", i);
		wsprintf(szKey, L"Single_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i < 60; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/SinglePlay/MouseOn/singleon%d.png", i);
		wsprintf(szKey, L"Single_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//멀티 플레이 애니메이션
	for (int i = 0; i < 50; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/MultyPlay/Animation/%d.png", i);
		wsprintf(szKey, L"Multy_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i < 50; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/MultyPlay/MouseOn/%d.png", i);
		wsprintf(szKey, L"Multy_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//에디터 애니메이션
	for (int i = 0; i < 85; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/Editor/Animation/editor%d.png", i);
		wsprintf(szKey, L"Editor_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i < 20; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/Editor/MouseOn/editoron%d.png", i);
		wsprintf(szKey, L"Editor_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Exit 애니메이션
	for (int i = 0; i < 50; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/Exit/Animation/exit%d.png", i);
		wsprintf(szKey, L"Exit_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i < 30; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/Exit/MouseOn/exiton%d.png", i);
		wsprintf(szKey, L"Exit_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
}

void CMenu::InitializeAnim()
{
	//Single 애니메이션 
	CObj* pAnimButton = CAbstractFactory<CAnimButton>::Create(150.f, 200.f);
	pAnimButton->Initialize();
	CAnimButton* pAnimBtn = dynamic_cast<CAnimButton*>(pAnimButton);
	if (pAnimBtn)
	{
		pAnimBtn->Set_NormalImage(L"Single_Normal_Anim_0");
		pAnimBtn->Set_NormalAnimation(L"Single_Normal_Anim_%d", 34);
		pAnimBtn->Set_HoverAnimation(L"Single_Hover_Anim_%d", 59);
		pAnimBtn->Set_ButtonType(L"Menu_Race");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pAnimButton);

	//Multiplay 애니메이션
	pAnimButton = CAbstractFactory<CAnimButton>::Create(200.f, 400.f);
	pAnimButton->Initialize();
	pAnimBtn = dynamic_cast<CAnimButton*>(pAnimButton);
	if (pAnimBtn)
	{
		pAnimBtn->Set_NormalImage(L"Multy_Normal_Anim_0");
		pAnimBtn->Set_NormalAnimation(L"Multy_Normal_Anim_%d", 49);
		pAnimBtn->Set_HoverAnimation(L"Multy_Hover_Anim_%d", 19);
		pAnimBtn->Set_ButtonType(L"Multiplay");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pAnimButton);

	//editor 애니메이션 
	pAnimButton = CAbstractFactory<CAnimButton>::Create(600.f, 200.f);
	pAnimButton->Initialize();
	pAnimBtn = dynamic_cast<CAnimButton*>(pAnimButton);
	if (pAnimBtn)
	{
		pAnimBtn->Set_NormalImage(L"Editor_Normal_Anim_0");
		pAnimBtn->Set_NormalAnimation(L"Editor_Normal_Anim_%d", 85);
		pAnimBtn->Set_HoverAnimation(L"Editor_Hover_Anim_%d", 19);
		pAnimBtn->Set_ButtonType(L"Edit");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pAnimButton);

	//Exit
	pAnimButton = CAbstractFactory<CAnimButton>::Create(550.f, 460.f);
	pAnimButton->Initialize();
	pAnimBtn = dynamic_cast<CAnimButton*>(pAnimButton);
	if (pAnimBtn)
	{
		pAnimBtn->Set_NormalImage(L"Exit_Normal_Anim_0");
		pAnimBtn->Set_NormalAnimation(L"Exit_Normal_Anim_%d", 49);
		pAnimBtn->Set_HoverAnimation(L"Exit_Hover_Anim_%d", 29);
		pAnimBtn->Set_ButtonType(L"Exit");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pAnimButton);
}

void CMenu::InitializeButton()
{
	//Single 버튼
	CObj* pButton = CAbstractFactory<CButton>::Create(200.f, 260.f);
	pButton->Initialize();
	CButton* pBtn = dynamic_cast<CButton*>(pButton);
	if (pBtn)
	{
		pBtn->Set_PngImages(L"Btn_Single_Normal", L"Btn_Single_Hover");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pButton);
	//Multiplayer 버튼
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
}
