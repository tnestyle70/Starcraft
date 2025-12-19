#pragma once
#include "CObj.h"
class CTile : public CObj
{
public:
	CTile();
	virtual ~CTile();

public:
	void	Set_DrawID(int iDrawID) { m_iDrawID = iDrawID; }
	void	Set_Option(int iOption) { m_iOption = iOption; }

	int Get_DrawID() { return m_iDrawID; }
	int Get_Option() { return m_iOption; }

public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;

private:
	int		m_iOption;
	int     m_iDrawID;
};


