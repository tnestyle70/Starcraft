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

	int Get_Cost() { return m_iCost; }
	int Get_Option() { return m_iOption; }

public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;

private:
	int		m_iOption; //지형 타입(충돌, 통과 여부)
	int		m_iCost; //이동 비용(가중치) A* 사용시 적용
};


