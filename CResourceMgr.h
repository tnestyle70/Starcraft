#pragma once
#include "Define.h"

struct ResourceState
{
	int mineral;
	int gas;
	int supplyUsed; //현재 인구수
	int supplyCap; //인구 최대 보급 한도
};

struct ResourceCost
{
	int mineral;
	int gas;
	int supply;//차지하는 인구수
};

class CResourceMgr
{
private:
	CResourceMgr();
	~CResourceMgr();
	CResourceMgr(const CResourceMgr&) = delete;
	CResourceMgr& operator=(const CResourceMgr&) = delete;
public:
	//자원, 인구수 초기화
	void Reset();
	//비용 조회 -> 생산 혹은 건설 가능 판단 
	ResourceState& GetState() { return m_eState; }
	//비용, 검사 지불
	bool CanProduce(ResourceCost& cost, bool bCheckUnit);
	bool TrySpend(ResourceCost& cost, bool bCheckUnit);
	//인구수 올리기
	void AddSupply(int supply);
	//자원 채취 후 획득
	void AddMineral(int amount);
	void AddGas(int amount);
	//보급
	bool CanAddSupply(int amount); //보급 한도 기준 생산 가능
	void AddUnitSupply(int amount);
	void SubtractSupply(int amount); //유닛 사망 후 보급량 감소
	void AddDepotSupply(int amount); //보급고 생산 후 보급량 증가
private:
	ResourceState m_eState;
public:
	static CResourceMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CResourceMgr;
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
private:
	static CResourceMgr* m_pInstance;
};