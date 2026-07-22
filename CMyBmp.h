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
public:
	int Get_Width() const;
	int Get_Height() const;
};
