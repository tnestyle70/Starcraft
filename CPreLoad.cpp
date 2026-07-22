#include "pch.h"
#include "CPreLoad.h"
#include "CInputMgr.h"
#include "CSceneMgr.h"
#include "CTimeMgr.h"
#include "CResourceLoader.h"

CPreLoad::CPreLoad()
{
}

CPreLoad::~CPreLoad()
{
	Release();
}

void CPreLoad::Initialize()
{
	CResourceLoader::Get_Instance()->LoadMenuResource();
	CResourceLoader::Get_Instance()->LoadMenuRaceResource();
	CResourceLoader::Get_Instance()->LoadProtossResource();
	CResourceLoader::Get_Instance()->LoadTerranResource();
	CResourceLoader::Get_Instance()->LoadZergResource();
}

int CPreLoad::Update()
{
	if (CInputMgr::Get_Instance()->KeyDownVK(VK_RETURN))
	{
		CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_MENU);
	}
	return 0;
}

void CPreLoad::Late_Update()
{
}

void CPreLoad::Render(HDC hDC)
{
}

void CPreLoad::Release()
{
}