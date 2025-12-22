#include "pch.h"
#include "CInputMgr.h"
#include "CScrollMgr.h"
#include <unordered_map>

//클래스의 싱글턴 포인터를 프로그램에서 한 번만 선언
CInputMgr* CInputMgr::m_pInst = nullptr;

CInputMgr::CInputMgr()
{
	memset(m_bPrevKey, 0, sizeof(m_bPrevKey));
	memset(m_bCurrentKey, 0, sizeof(m_bCurrentKey));
}

CInputMgr::~CInputMgr()
{
}

CInputMgr* CInputMgr::Get_Instance()
{
	if (m_pInst == nullptr)
	{
		m_pInst = new CInputMgr;
	}
	return m_pInst;
}

void CInputMgr::Destroy_Instance()
{
	if (m_pInst)
	{
		delete m_pInst;
		m_pInst = nullptr;
	}
}

void CInputMgr::Initialize()
{
	//논리키와 실제키 매핑
	m_iVKCode[LEFT_MOUSE] = VK_LBUTTON;
	m_iVKCode[RIGHT_MOUSE] = VK_RBUTTON;
	m_iVKCode[S_KEY] = 'S';
	m_iVKCode[B_KEY] = 'B';
	m_iVKCode[G_KEY] = 'G';
	m_iVKCode[ESCAPE0] = '0';
}

void CInputMgr::Update()
{
	GetCursorPos(&m_ptMouse);
	ScreenToClient(g_hWnd, &m_ptMouse);
	//이전 프레임 상태 백업
	memcpy(m_bPrevKey, m_bCurrentKey, sizeof(m_bPrevKey));
	//이번 프레임 상태 갱신
	for (int i = 0; i < eKey::KEY_END; ++i)
	{
		int iVK = m_iVKCode[i];
		SHORT sState = GetAsyncKeyState(iVK);
		//최상위 비트가 눌렸는지 여부 확인
		m_bCurrentKey[i] = (sState & 0x8000) != 0;
	}
}

Vec2 CInputMgr::GetWorldMouse()
{
	//스크롤 값으로 월드 마우스 좌표 반환 
	float fScrX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrY = CScrollMgr::Get_Instance()->Get_ScrollY();
	
	GetCursorPos(&m_ptMouse);
	ScreenToClient(g_hWnd, &m_ptMouse);

	 printf("client(%ld,%ld) scroll(%.1f,%.1f) world(%.1f,%.1f)\n",
     m_ptMouse.x, m_ptMouse.y, fScrX, fScrY, m_ptMouse.x + fScrX, m_ptMouse.y + fScrY);

	Vec2 vDir{ m_ptMouse.x + fScrX, m_ptMouse.y + fScrY };

	return vDir;
}
//누르고 있는 동안
bool CInputMgr::KeyPress(eKey eKey) const
{
	return m_bCurrentKey[eKey];
}
//한 프레임 눌린 순간
bool CInputMgr::KeyDown(eKey eKey) const
{
	return m_bCurrentKey[eKey] && !m_bPrevKey[eKey];
}
//뗀 순간
bool CInputMgr::KeyUp(eKey eKey) const
{
	return !m_bCurrentKey[eKey] && m_bPrevKey[eKey];
}

bool CInputMgr::KeyPressVK(int vkCode) const
{
	SHORT sState = GetAsyncKeyState(vkCode);
	return (sState & 0x8000) != 0;
}

bool CInputMgr::KeyDownVK(int vkCode) const
{
	// VK 전용 배열 필요
	static std::unordered_map<int, bool> prevState;
	bool current = (GetAsyncKeyState(vkCode) & 0x8000) != 0;
	bool prev = prevState[vkCode];
	prevState[vkCode] = current;
	return current && !prev;
}

bool CInputMgr::KeyUpVK(int vkCode) const
{
	static std::unordered_map<int, bool> prevState;
	bool current = (GetAsyncKeyState(vkCode) & 0x8000) != 0;
	bool prev = prevState[vkCode];
	prevState[vkCode] = current;
	return !current && prev;
}