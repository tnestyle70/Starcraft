#pragma once
#include "Define.h"

class CUpgradeMgr
{
private:
	int m_iInfantryAttackLevel = 0;
	int m_iInfantryArmorLevel = 0;
	int m_iVehicleAttackLevel = 0;
	int m_iVehicleArmorLevel = 0;
	int m_iShipAttackLevel = 0;
	int m_iShipArmorLevel = 0;
public:
	int GetAttackBonus(eUnitType type);
	int GetArmorBonus(eUnitType type);
private:
	CUpgradeMgr();
	~CUpgradeMgr();
public:
	void Intialize();
	void Release();
public:
	static CUpgradeMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CUpgradeMgr;
		return m_pInstance;
	}
	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	static CUpgradeMgr* m_pInstance;
};