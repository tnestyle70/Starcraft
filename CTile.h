#pragma once
#include "CObj.h"
class CTile : public CObj
{
public:
	CTile();
	virtual ~CTile();

public:
	void	Set_Cost(int iCost) { m_iCost = iCost; }
	void	Set_Option(int iOption) { m_iOption = iOption; }
	void	Set_Expansion(bool expansion) { m_bExpansion = expansion; }

	int Get_Cost() { return m_iCost; }
	int Get_Option() { return m_iOption; }
	bool Get_Expansion() { return m_bExpansion; }

public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;

private:
	int		m_iOption; //지형 타입(충돌, 통과 여부)
	int		m_iCost; //이동 비용(가중치) A* 사용시 적용
	bool	m_bExpansion = false; //자원 주변 건설 가능 여부 확인
};


