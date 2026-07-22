#include "pch.h"
#include "CMyBmp.h"

CMyBmp::CMyBmp()
{
	m_eType = eImageType::BMP;
}

CMyBmp::~CMyBmp()
{
	Release();
}

void CMyBmp::Load_Image(const TCHAR* pFilePath)
{
	HDC hDC = GetDC(g_hWnd);
	//매개 변수로 전달한 dc와 호환되는 dc를 할당 
	m_hMemDC = CreateCompatibleDC(hDC);

	ReleaseDC(g_hWnd, hDC);
	
	m_hBitmap = (HBITMAP)LoadImage(NULL,			// 프로그램 인스턴스 핸들
		pFilePath,		// 파일의 경로
		IMAGE_BITMAP,	// 파일의 타입
		0, 0,			// 가로, 세로 사이즈
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	//	LR_LOADFROMFILE : 파일로부터 이미지를 읽어들임
	//	LR_CREATEDIBSECTION : 읽어온 파일을 DIB 형태로 변환

		//SelectObject : 준비한 dc를 gdi 오브젝트로 불러온 비트맵에 선택하여 적용

	m_hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);
}

void CMyBmp::Release()
{
	SelectObject(m_hMemDC, m_hOldBitmap);
	DeleteObject(m_hBitmap);
	DeleteDC(m_hMemDC);
	m_hMemDC = nullptr;
	m_hBitmap = nullptr;
	m_hOldBitmap = nullptr;
}

int CMyBmp::Get_Width() const
{
	if (!m_hBitmap)
	{
		return 0;
	}
	BITMAP bm;
	GetObject(m_hBitmap, sizeof(BITMAP), &bm);
	return bm.bmWidth;
}

int CMyBmp::Get_Height() const
{
	if (!m_hBitmap)
	{
		return 0;
	}
	BITMAP bm;
	GetObject(m_hBitmap, sizeof(BITMAP), &bm);
	return bm.bmHeight;
}
