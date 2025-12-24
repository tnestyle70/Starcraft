#pragma once
#include "Define.h"

enum eKey
{
	LEFT_MOUSE, RIGHT_MOUSE, S_KEY, B_KEY, G_KEY, ESCAPE,
	ESCAPE0 ,KEY_END
};

class CInputMgr
{
public:
	static CInputMgr* Get_Instance();
	static void Destroy_Instance();
public:
	void Initialize();
	void Update();
public:
	Vec2 GetWorldMouse();

	float GetMouseX() { return m_ptMouse.x; }
	float GetMouseY() { return m_ptMouse.y; }
public:
	bool KeyPress(eKey eKey) const;
	bool KeyDown(eKey eKey) const;
	bool KeyUp(eKey eKey) const;
public:
	bool KeyPressVK(int vkCode) const;
	bool KeyDownVK(int vkCode) const;
	bool KeyUpVK(int vkCode) const;
private:
	CInputMgr();
	~CInputMgr();
private:
	POINT m_ptMouse;
	//싱글턴 패턴 - 전역에서 딱 하나 존재하는 input포인터 멤버 변수, 
	//이걸로 다 접근해서 사용
	static CInputMgr* m_pInst;
	//이전, 현재 프레임 키
	bool m_bPrevKey[eKey::KEY_END];
	bool m_bCurrentKey[eKey::KEY_END];
	//가상 키와 논리키 매핑
	int m_iVKCode[eKey::KEY_END];
};
