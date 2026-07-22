#include "pch.h"
#include "CLogo.h"
#include "CInputMgr.h"
#include "CSceneMgr.h"
#include "CTimeMgr.h"
#include "CResourceLoader.h"

CLogo::CLogo()
{
}

CLogo::~CLogo()
{
	Release();
}

void CLogo::Initialize()
{
	m_hVideo = MCIWndCreate(g_hWnd,	// 부모 창 핸들
		nullptr,	// mci 윈도우를 사용하는 인스턴스 핸들
		WS_CHILD | WS_VISIBLE | MCIWNDF_NOPLAYBAR,  // 자식 창 | 그 즉시 재생 | 플레이바 제거
		L"../Video/Logo.wmv");	// 파일 이름을 포함한 경로

	if (m_hVideo)  // 생성 실패 체크
	{
		MoveWindow(m_hVideo, 0, 0, WINCX, WINCY, FALSE);
		MCIWndPlay(m_hVideo);
	}

	MCIWndPlay(m_hVideo);

	m_eState = eLogoState::VIDEO;
}

int CLogo::Update()
{
	//이미 씬 전환 중이면 아무것도 하지 않음
	if (m_bSceneChanging)
		return 0;
	//핸들 유효성 체크
	if (!m_hVideo)
		return 0;

	//비디오 재생 완료 체크
	if (MCIWndGetLength(m_hVideo) <= MCIWndGetPosition(m_hVideo))
	{
		m_bSceneChanging = true;
		MCIWndStop(m_hVideo);       // 재생 중지
		MCIWndClose(m_hVideo);      // 미디어 닫기
		MCIWndDestroy(m_hVideo);    // ← 윈도우 파괴
		m_hVideo = nullptr;         // ← 핵심: 핸들 무효화
		CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_MENU);
	}
	//else if (CInputMgr::Get_Instance()->KeyDownVK(VK_RETURN))
	//{
	//	ShowWindow(m_hVideo, SW_HIDE);
	//	MCIWndStop(m_hVideo);
	//	CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_MENU);
	//}
	return 0;
}

void CLogo::Late_Update()
{
}

void CLogo::Render(HDC hDC)
{
}

void CLogo::Release()
{
	if (m_hVideo)
	{
		MCIWndStop(m_hVideo);       // 재생 중지
		MCIWndClose(m_hVideo);      // 미디어 닫기
		MCIWndDestroy(m_hVideo);    // ← 윈도우 파괴
		m_hVideo = nullptr;         // ← 핵심: 핸들 무효화
	}
}