#pragma once
#include "CMyImage.h"

class CMyBmp : public CMyImage
{
public:
	CMyBmp();
	virtual ~CMyBmp();
public:
	void	Load_Image(const TCHAR* pFilePath) override;
	void	Release() override;

//private:
//	HDC		m_hMemDC;
//	HBITMAP	m_hBitmap;
//	HBITMAP	m_hOldmap;
};
