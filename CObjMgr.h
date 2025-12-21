#pragma once

#include "CObj.h"

class CUnit;
class CBuilding;
class CButton;

class CObjMgr
{
private:
	CObjMgr();
	CObjMgr(const CObjMgr& rhs) = delete;
	CObjMgr& operator=(CObjMgr& rObj) = delete;
	~CObjMgr();

public:
	void	Delete_Obj(OBJID eID);
public:
	void	Add_Object(OBJID eID, CObj* pObj);
	void	Update();
	void	Late_Update();
	void	Render(HDC hDC);
	void	Release();
public:
	CObj* PickObjAt(const Vec2& vWorldPos);
public:
	CButton CreateButton(float fX, float fY, float fCX, float fCY);
	CButton* CreateButton(const wchar_t* framekey, float fX, float fY, const wchar_t* atlasKey, int iScrY);
public:
	vector<CUnit*>& GetUnits() { return m_vecUnits; }
	vector<CBuilding*>& GetBuildings() { return m_vecBuildings; }
private:
	vector<CUnit*> m_vecUnits;
	vector<CBuilding*> m_vecBuildings;
private:
	static CObjMgr* m_pInstance;

	list<CObj*>	m_ObjList[OBJ_END];
	//렌더링을 위한 Y소팅 순서 리스트
	vector<CObj*> m_vecRenderGroup[(int)RENDER_END];
	list<CObj*>	m_RenderList[RENDER_END];
public:
	static CObjMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CObjMgr;
		}

		return m_pInstance;
	}

	static void	Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
};


