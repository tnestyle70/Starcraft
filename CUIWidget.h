#pragma once
#include "CObj.h"

class CUIWidget : public CObj
{
protected:
	CUIWidget() {};
	virtual ~CUIWidget() {};
public:
	virtual void Initialize()	PURE;
	virtual int	 Update()		PURE;
	virtual void Late_Update()	PURE;
	virtual void Render(HDC hDC)PURE;
	virtual void Release()		PURE;
public:
	virtual bool Hover() PURE;
};