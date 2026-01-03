#pragma once
#include "CUnit.h"

class CBattleCruiser : public CUnit
{
public:
	CBattleCruiser();
	virtual ~CBattleCruiser();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
protected:
	bool UpdateAttack(Order& order) override;
public:
	//공격 
	void Fire_Bullet(CObj* pTarget);
	void Fire_Yamato(CObj* pTarget);
	//에너지 
	int Get_Energy() const { return m_iEnergy; }
	//bool Can_Use_Yamato() const { return m_iEnergy >= m_iYamatoCost; }
private:
	//에너지 관련
	int m_iEnergy;
	int m_iMaxEnergy;
	//야마토
	int m_iYamatoCost;
};


