#include "pch.h"
#include "CUpgradeMgr.h"

CUpgradeMgr* CUpgradeMgr::m_pInstance = nullptr;

CUpgradeMgr::CUpgradeMgr()
{
}

CUpgradeMgr::~CUpgradeMgr()
{
	Release();
}

void CUpgradeMgr::Intialize()
{
}

void CUpgradeMgr::Release()
{
}

int CUpgradeMgr::GetAttackBonus(eUnitType type)
{
	if (type == eUnitType::MARINE || type == eUnitType::FIREBAT || type == eUnitType::GHOST)
	{
		return m_iInfantryAttackLevel;
	}
	else if (type == eUnitType::VULTURE || type == eUnitType::TANK)
	{
		return m_iVehicleAttackLevel;
	}
	else if (type == eUnitType::BATTLECRUISER)
	{
		return m_iShipAttackLevel;
	}
}

int CUpgradeMgr::GetArmorBonus(eUnitType type)
{
	if (type == eUnitType::MARINE || type == eUnitType::FIREBAT || type == eUnitType::GHOST)
	{
		return m_iInfantryArmorLevel;
	}
	else if (type == eUnitType::VULTURE || type == eUnitType::TANK)
	{
		return m_iVehicleArmorLevel;
	}
	else if (type == eUnitType::BATTLECRUISER)
	{
		return m_iShipArmorLevel;
	}
}
