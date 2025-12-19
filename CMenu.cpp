#include "pch.h"
#include "CMenu.h"
#include "CButton.h"
#include "CBmpMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"

CMenu::CMenu()
{
}

CMenu::~CMenu()
{
}

void CMenu::Initialize()
{
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MyFolder/Button/Single_Button00.bmp", L"Start");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MyFolder/Button/Editor_Button00.bmp", L"Edit");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MyFolder/Button/Exit_Button00.bmp", L"Exit");

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MyFolder/MenuBack.bmp", L"Menu");

	CObj* pButton = CAbstractFactory<CButton>::Create(200.f, 200.f, 100, 22);
	pButton->Initialize();
	pButton->Set_FrameKey(L"Start");
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pButton);

	pButton = CAbstractFactory<CButton>::Create(500.f, 200.f, 201, 27);
	pButton->Initialize();
	pButton->Set_FrameKey(L"Edit");
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pButton);

	pButton = CAbstractFactory<CButton>::Create(600.f, 400.f, 56, 28);
	pButton->Initialize();
	pButton->Set_FrameKey(L"Exit");
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pButton);
}

int CMenu::Update()
{
	CObjMgr::Get_Instance()->Update();

	return 0;
}

void CMenu::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();
}

void CMenu::Render(HDC hDC)
{
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
}

void CMenu::Release()
{
	CObjMgr::Get_Instance()->Delete_Obj(OBJ_UI);
}
