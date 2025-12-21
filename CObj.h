#pragma once
#include "Define.h"

class CObj
{
public:
	CObj();
	virtual ~CObj();
public:
	virtual void Initialize()	PURE;
	virtual int	 Update()		PURE;
	virtual void Late_Update()	PURE;
	virtual void Render(HDC hDC)PURE;
	virtual void Release()		PURE;
public:
	virtual bool IsDead() const { return false; };
	INFO Get_Info() { return m_tInfo; }
	RECT* Get_Rect() { return &m_tRect; }
	Vec2 Get_Pos() { return { m_tInfo.fX, m_tInfo.fY }; }

	void Set_Pos(float fX, float fY) { m_tInfo.fX = fX, m_tInfo.fY = fY; }
	void Set_Size(float fCX, float fCY) { m_tInfo.fCX = fCX, m_tInfo.fCY = fCY; }

	void Set_FrameKey(const TCHAR* pFrameKey) { m_pFrameKey = pFrameKey; }

	void Update_Rect();
	void Move_Frame();

	RENDERID		Get_RenderID() const { return m_eRender; }
public:
	int m_iZOrder = 0;
	virtual int GetSortY() const { return m_tRect.bottom; } 
protected:
	INFO m_tInfo; //상태
	RECT m_tRect;
	FRAME m_tFrame; //애니메이션
	RENDERID m_eRender;
	const TCHAR* m_pFrameKey;
	int m_iDrawID;
};