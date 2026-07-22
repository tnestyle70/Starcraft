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
private:
	int m_iArmorUpgrade = 0;
	int m_iAttackUpgrade = 0;
public:
	void SetArmorUpgrade(int upgrade) { m_iArmorUpgrade = upgrade; }
	int GetArmorUpgrade() { return m_iArmorUpgrade; }
	void SetAttackUpgrade(int upgrade) { m_iArmorUpgrade = upgrade; }
	int GetAttackUpgrade() { return m_iAttackUpgrade; }
private:
	int m_iDestroyCount = 0;
	bool m_bStartZergRush = false;
	bool m_bStartTerranRush = false;
	bool m_bStartProtossRush = false;
	bool m_bLastRush = false;
public:
	int GetDestroyCount() { return m_iDestroyCount; }
	void IncreaseDestroyCount() { m_iDestroyCount++; }

	bool IsStartZergRush() { return m_bStartZergRush; }
	void SetZergRush(bool start) { m_bStartZergRush = start; }

	bool IsStartTerranRush() { return m_bStartTerranRush; }
	void SetTerranRush(bool start) { m_bStartTerranRush = start; }

	bool IsStartProtossRush() { return m_bStartProtossRush; }
	void SetProtossRush(bool start) { m_bStartProtossRush = start; }

	bool IsLastRush() { return m_bLastRush; }
	void SetLastRush(bool start) { m_bLastRush = start; }

private:
	//배럭, 군수공장, 아카데미 건설 여부 체크
	bool m_bBarrackBuilt = false;
	bool m_bFactoryBuilt = false;
	bool m_bAcademyBuilt = false;
	//핵 미사일, 야마토, 스팀팩
	bool m_bMissileReady = false;
	bool m_bYamatoReady = false;
	bool m_bStimPack = false;
	//프로토스 관문, 로보틱스, 스타게이트, 차원 관문
	bool m_bGateBuilt = false;
	bool m_bRoboticsBuilt = false;
	bool m_bStarGateBuilt = false;
	bool m_bWarpGate = false;
	//저그 - 스포닝풀, 히드라덴, 뮤탈덴, 울트라덴
	bool m_bSpawningPool = false;
	bool m_bHydraliskDen = false;
	bool m_bSpire = false;
	bool m_bUltraliskDen = false;
public:
	//저그
	bool IsSpawningPoolBuilt() { return m_bSpawningPool; }
	void SetSpawningPoolBuilt(bool built) { m_bSpawningPool = built; }

	bool IsHydraliskDenBuilt() { return m_bHydraliskDen; }
	void SetHydraliskDenBuilt(bool built) { m_bHydraliskDen = built; }

	bool IsSpireBuilt() { return m_bSpire; }
	void SetSpireBuilt(bool built) { m_bSpire = built; }

	bool IsUltraliskDenBuilt() { return m_bUltraliskDen; }
	void SetUltraliskDenBuilt(bool built) { m_bUltraliskDen = built; }
	//테란
	bool IsBarrackBuilt() { return m_bBarrackBuilt; } //배럭 건설
	void SetBarrackBuilt(bool built) { m_bBarrackBuilt = built; }
	bool IsFactoryBuilt() { return m_bFactoryBuilt; } //팩토리 건설
	void SetFactoryBuilt(bool built) { m_bFactoryBuilt = built; }
	bool IsAcademyBuilt() { return m_bAcademyBuilt; } //아카데미 건설
	void SetAcademyBuilt(bool built) { m_bAcademyBuilt = built; }
	bool IsMissileReady() { return m_bMissileReady; } //핵 미사일 준비
	void SetMissileReady(bool ready) { m_bMissileReady = ready; }
	bool IsYamatoReady() { return m_bYamatoReady; } //야마토 준비
	void SetYamatoReady(bool ready) { m_bYamatoReady = ready; }
	bool IsStimPackReady() { return m_bStimPack; } //스팀팩 준비
	void SetStimPackReady(bool ready) { m_bStimPack = ready; }
	//프로토스
	bool IsWarpGateReady() { return m_bWarpGate; } //프로토스 차원 관문
	void SetWarpGateReady(bool ready) { m_bWarpGate = ready; }	
	bool IsGateBuilt() { return m_bGateBuilt; } //프로토스 관문
	void SetGateBuilt(bool built) { m_bGateBuilt = built; }
	bool IsRoboticsBuilt() { return m_bRoboticsBuilt; } //프로토스 로보틱스 
	void SetRoboticsBuilt(bool built) { m_bRoboticsBuilt = built; }
	bool IsStargateBuilt() { return m_bStarGateBuilt; } //프로토스 스타게이트
	void SetStargateBuilt(bool built) { m_bStarGateBuilt = built; }
public:
	void	Delete_Obj(OBJID eID);
	void Delete_All_Obj();
public:
	void	Add_Object(OBJID eID, CObj* pObj);
	void	Update();
	void	Late_Update();
	void	Render(HDC hDC);
	void	CleanUpDeadObject();
	void RemoveFromAllLists(CObj* pObj);
	void RemoveTargetFromAllOrders(CObj* pTarget);
	void	Release();
public:
	CObj* PickObjAt(const Vec2& vWorldPos);
	CObj* PickUnitAt(const Vec2& vWorldPos);
	CObj* PickEnemyAt(const Vec2& vWorldPos);
	OBJID CheckHoveredObject(const Vec2& vMousePos);
	bool IsMouseOver(CObj* pObj, const Vec2& vMousePos);
	void MoveObject(CObj* pObj, OBJID fromList, OBJID toList);
public:
	CButton CreateButton(float fX, float fY, float fCX, float fCY);
	//CButton* CreateButton(const wchar_t* framekey, float fX, float fY, const wchar_t* atlasKey, int iScrY);
public:
	vector<CUnit*>& GetUnits() { return m_vecUnits; }
	vector<CBuilding*>& GetBuildings() { return m_vecBuildings; }
	vector<CObj*>& GetEnemies() { return m_vecEnemies; }
public:
	list<CObj*>& Get_ObjList(OBJID eID);
	list<CObj*>* Get_ObjListPointer(OBJID eID);
private:
	vector<CUnit*> m_vecUnits;
	vector<CBuilding*> m_vecBuildings;
	vector<CObj*> m_vecEnemies;
private:
	static CObjMgr* m_pInstance;

	list<CObj*>	m_ObjList[OBJ_END];
	//현재 프레임에 죽어서 마지막에 delete할 vector 컨테이너
	vector<CObj*> m_PendingDelete; 
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


