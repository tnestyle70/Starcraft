#include "pch.h"
#include "CMyImage.h"

CMyImage::CMyImage() : 
	m_hMemDC(nullptr)
	,m_hBitmap(nullptr)
	,m_hOldBitmap(nullptr) 
	,m_eType(eImageType::BMP)
{
}
