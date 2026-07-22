#include "pch.h"
#include "CMenuRace.h"
#include "CButton.h"
#include "CAnimButton.h"
#include "CBmpMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CCursorMgr.h"
#include "CSoundMgr.h"
#include "CTimeMgr.h"
#include "CSceneMgr.h"

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
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu2/BackGround.png", L"Menu_Race");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu2/BackGround_2.png", L"MenuRace");

	LoadResource();
	InitializeButton();
	InitializeAnim();
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

	if (m_bStart)
	{
		float dt = CTimeMgr::Get_Instance()->GetDT();
		m_fSoundDelay += dt;
		if (m_fSoundDelay >= m_fSoundDuration)
		{
			m_fSoundDelay = 0.f;
			CSoundMgr::Get_Instance()->PlaySound(L"BGM/CountDown.wav", SOUND_EFFECT, 0.4f);
		}
		m_fStartDelay += dt;
		if (m_fStartDelay >= m_fDelayDuartion)
		{
			CSoundMgr::Get_Instance()->StopSound(SOUND_BGM);
			CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_STAGE);
		}
	}
}

void CMenuRace::Render(HDC hDC)
{
	CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(L"MenuRace");
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
	//CObjMgr::Get_Instance()->Destroy_Instance();
}

void CMenuRace::LoadResource()
{
	//프로토스 버튼
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
	//테란 버튼
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
	//저그 버튼
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
}

void CMenuRace::InitializeAnim()
{
	//종족 애니메이션 생성

	//프로토스
	CObj* pAnimButton = CAbstractFactory<CAnimButton>::Create(410.f, 210.f);
	pAnimButton->Initialize();
	CAnimButton* pAnimBtn = dynamic_cast<CAnimButton*>(pAnimButton);
	if (pAnimBtn)
	{
		pAnimBtn->Set_MenuScene(this);
		pAnimBtn->Set_NormalImage(L"Protoss_Normal_Anim_0");
		pAnimBtn->Set_NormalAnimation(L"Protoss_Normal_Anim_%d", 69);
		pAnimBtn->Set_HoverAnimation(L"Protoss_Hover_Anim_%d", 30);
		pAnimBtn->Set_ButtonType(L"Protoss");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pAnimButton);
	 
	//테란
	pAnimButton = CAbstractFactory<CAnimButton>::Create(120.f, 310.f);
	pAnimButton->Initialize();
	pAnimBtn = dynamic_cast<CAnimButton*>(pAnimButton);
	if (pAnimBtn)
	{
		pAnimBtn->Set_MenuScene(this);
		pAnimBtn->Set_NormalImage(L"Terran_Normal_Anim_0");
		pAnimBtn->Set_NormalAnimation(L"Terran_Normal_Anim_%d", 77);
		pAnimBtn->Set_HoverAnimation(L"Terran_Hover_Anim_%d", 30);
		pAnimBtn->Set_ButtonType(L"Terran");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pAnimButton);

	//저그
	pAnimButton = CAbstractFactory<CAnimButton>::Create(690.f, 290.f);
	pAnimButton->Initialize();
	pAnimBtn = dynamic_cast<CAnimButton*>(pAnimButton);
	if (pAnimBtn)
	{
		pAnimBtn->Set_MenuScene(this);
		pAnimBtn->Set_NormalImage(L"Zerg_Normal_Anim_0");
		pAnimBtn->Set_NormalAnimation(L"Zerg_Normal_Anim_%d", 69);
		pAnimBtn->Set_HoverAnimation(L"Zerg_Hover_Anim_%d", 19);
		pAnimBtn->Set_ButtonType(L"Zerg");
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_UI, pAnimButton);
}

void CMenuRace::InitializeButton()
{
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/single_button1.png", L"Btn_Cancle_Hover");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/cancle_button.png", L"Btn_Cancle_Normal");
}
