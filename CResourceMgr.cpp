#include "pch.h"
#include "CResourceMgr.h"

CResourceMgr* CResourceMgr::m_pInstance = nullptr;

CResourceMgr::CResourceMgr()
{
}

CResourceMgr::~CResourceMgr()
{
}

void CResourceMgr::Reset()
{
	m_eState.mineral = 10000000;
	m_eState.gas = 100000000;
	m_eState.supplyUsed = 0;
	m_eState.supplyCap = 1000;
}
//CheckSupply는 건물인지 유닛인지 판단
bool CResourceMgr::CanProduce(ResourceCost& cost, bool bCheckUnit)
{
	//현재 비용과 유닛, 건물의 생산 비용을 비교해서 건설 가능한지 여부 판단
	if (m_eState.mineral < cost.mineral) return false;
	if (m_eState.gas < cost.gas) return false;
	
	if (bCheckUnit)
	{
		if (m_eState.supplyUsed + cost.supply > m_eState.supplyCap)
			return false;
	}
	return true;
}

bool CResourceMgr::TrySpend(ResourceCost& cost, bool bCheckUnit)
{
	if (!CanProduce(cost, bCheckUnit)) return false;

	m_eState.mineral -= cost.mineral;
	m_eState.gas -= cost.gas;
	if (bCheckUnit && cost.supply > 0)
		m_eState.supplyUsed += cost.supply;
	//MakeDirty()
	return true;
}

void CResourceMgr::AddMineral(int amount)
{
	m_eState.mineral += amount;
	return;
}

void CResourceMgr::AddGas(int amount)
{
	m_eState.gas += amount;
	return;
}

bool CResourceMgr::CanAddSupply(int amount)
{
	//최대 보급량이랑 비교해서 유닛 생산 가능한지 판단
	return (m_eState.supplyUsed + amount <= m_eState.supplyCap);
}

void CResourceMgr::AddUnitSupply(int amount)
{
	if (!CanAddSupply(amount)) return;
	//생산 가능할 경우 보급량 증가
	m_eState.supplyUsed += amount;
	return;
}

void CResourceMgr::SubtractSupply(int amount)
{
	//유닛이 죽었을 경우 인구수 감소
	m_eState.supplyUsed -= amount;
	return;
}

void CResourceMgr::AddDepotSupply(int amount)
{
	//보급고 생산후 총 인구수 증가
	m_eState.supplyCap += amount;
}
