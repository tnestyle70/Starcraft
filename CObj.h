#pragma once
#include "Define.h"

enum class eTeamType
{
	ALLY, ENEMY
};

enum class eUnitLayer
{
	GROUND, AIR
};

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
	virtual void TakeDamage(int iAttackDamage) {};
	virtual void Heal(int iHeal) {};
	INFO Get_Info() { return m_tInfo; }
	RECT* Get_Rect() { return &m_tRect; }
	RECT GetWorldRect();
	Vec2 Get_Pos() { return { m_tInfo.fX, m_tInfo.fY }; }

	void Set_Pos(float fX, float fY) { m_tInfo.fX = fX, m_tInfo.fY = fY; }
	void Set_Size(float fCX, float fCY) { m_tInfo.fCX = fCX, m_tInfo.fCY = fCY; }

	void Set_FrameKey(const TCHAR* pFrameKey) { m_pFrameKey = pFrameKey; }

	void Update_Rect();
	void Move_Frame();

	bool Is_Dead() { return m_bDead; }
	void Set_Dead() { m_bDead = true; }

	eTeamType GetTeamType() { return m_eTeamType; }
	void SetTeamType(eTeamType type) { m_eTeamType = type; }

	eUnitLayer GetLayer() { return m_eLayer; }

	RENDERID		Get_RenderID() const { return m_eRender; }
public:
	int m_iZOrder = 0;
	virtual int GetSortY() const { return m_tRect.bottom; } 
public:
	bool IsSelected() { return m_bSelected; }
	void SetSelected(bool bSelected){ m_bSelected = bSelected; }
	bool IsSelectable() { return m_bSelectable; }
	void SetSelectable(bool selectable) { m_bSelectable = selectable; }
	Vec2 GetVelocity() { return m_velocity; }
	void SetVelocity(const Vec2& v) { m_velocity = v; }
public:
	eRaceType GetOriginalRace() { return m_eOriginalRace; }
	eRaceType GetCurrentRace() { return m_eCurrentRace; }
	void SetCurrentRace(eRaceType type) { m_eCurrentRace = type; }
protected:
	eRaceType m_eOriginalRace;
	eRaceType m_eCurrentRace;
	//선택 상태 CObj로 올리기
	bool m_bSelected;
	bool m_bSelectable = true;
	INFO m_tInfo; //상태
	RECT m_tRect;
	bool m_bDead;
	FRAME m_tFrame; //애니메이션
	RENDERID m_eRender;
	const TCHAR* m_pFrameKey;
	int m_iDrawID;
	Vec2 m_velocity;
	eTeamType m_eTeamType;
	eUnitLayer m_eLayer = eUnitLayer::GROUND; //기본 유닛 Layer 상태 지상으로 설정
};