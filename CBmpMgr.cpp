#include "pch.h"
#include "CBmpMgr.h"

CBmpMgr* CBmpMgr::m_pInstance = nullptr;

CBmpMgr::CBmpMgr()
{
}

CBmpMgr::~CBmpMgr()
{
	Release();
}

void CBmpMgr::Insert_Bmp(const TCHAR* pFilePath, const TCHAR* pImgKey)
{
	auto		iter = find_if(m_mapBit.begin(), m_mapBit.end(), tagFinder(pImgKey));

	if (iter == m_mapBit.end())
	{
		CMyBmp* pBmp = new CMyBmp;
		pBmp->Load_Bmp(pFilePath);

		m_mapBit.insert({ pImgKey, pBmp });
	}

}

HDC CBmpMgr::Find_Image(const TCHAR* pImgKey)
{
	auto		iter = find_if(m_mapBit.begin(), m_mapBit.end(), tagFinder(pImgKey));

	if (iter == m_mapBit.end())	// 키값에  해당하는 원소가 없을 경우 end를 리턴
	{
		return nullptr;
	}
	return iter->second->Get_MemDC();
}

HBITMAP CBmpMgr::Find_Bitmap(const TCHAR* pImgKey)
{
    auto iter = find_if(m_mapBit.begin(), m_mapBit.end(), tagFinder(pImgKey));

    if (iter == m_mapBit.end())
        return nullptr;
    return iter->second->Get_Bitmap();

    return HBITMAP();
}

void CBmpMgr::Release()
{
	for_each(m_mapBit.begin(), m_mapBit.end(), DeleteMap());
	m_mapBit.clear();
}

void CBmpMgr::Render_Alpha(const TCHAR* pImageKey, const TCHAR* pAlphaKey, 
	HDC hDC, int x, int y, int width, int height)
{
	//Image와 Alpha 채널을 불러와서 BLENDOPTION 값을 줘서 렌더링z
	HDC hColorDC = Find_Image(pImageKey);
	HDC hAlphaDC = Find_Image(pAlphaKey);

	if (!hColorDC || !hAlphaDC) return;

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 180; //투명도 설정 옵션
	bf.AlphaFormat = 0;
	//AlphaBlend로 렌더링
	AlphaBlend(hDC, x, y, width, height,
		hColorDC, 0, 0, width, height, bf);
}

void CBmpMgr::Render_Alpha_Simple(const TCHAR* pImageKey, HDC hDC, 
    int x, int y, int width, int height)
{
    // m_mapBit에서 CMyBmp 객체 가져오기
    auto iter = m_mapBit.find(pImageKey);
    if (iter == m_mapBit.end()) return;

    CMyBmp* pBmp = iter->second;
    HDC hMemDC = pBmp->Get_MemDC();  // CMyBmp의 DC 가져오기
    HBITMAP hBitmap = pBmp->Get_Bitmap();  // 비트맵 핸들 가져오기

    // 실제 비트맵 크기 확인
    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 180;
    bf.AlphaFormat = 0;

    // 원본 크기 → 목표 크기로 스케일링
    AlphaBlend(hDC, x, y, width, height,
        hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, bf);
}

void CBmpMgr::Render_Alpha_Tint(const TCHAR* pImageKey, const TCHAR* pAlphaKey, HDC hDC, 
	int x, int y, int width, int height, COLORREF tintColor)
{
    // 컬러 이미지 가져오기
    auto iterColor = m_mapBit.find(pImageKey);
    if (iterColor == m_mapBit.end()) return;

    CMyBmp* pColorBmp = iterColor->second;
    HDC hColorDC = pColorBmp->Get_MemDC();
    HBITMAP hColorBitmap = pColorBmp->Get_Bitmap();

    // 알파 마스크 가져오기
    auto iterAlpha = m_mapBit.find(pAlphaKey);
    if (iterAlpha == m_mapBit.end()) return;

    CMyBmp* pAlphaBmp = iterAlpha->second;
    HDC hAlphaDC = pAlphaBmp->Get_MemDC();
    HBITMAP hAlphaBitmap = pAlphaBmp->Get_Bitmap();

    // 실제 비트맵 크기 확인
    BITMAP bm;
    GetObject(hColorBitmap, sizeof(BITMAP), &bm);
    int srcWidth = bm.bmWidth;
    int srcHeight = bm.bmHeight;

    // === 1단계: 색상 틴트 적용 ===
    HDC hTintDC = CreateCompatibleDC(hDC);
    HBITMAP hTintBmp = CreateCompatibleBitmap(hDC, srcWidth, srcHeight);
    HBITMAP hOldTint = (HBITMAP)SelectObject(hTintDC, hTintBmp);

    // 원본 이미지 복사
    BitBlt(hTintDC, 0, 0, srcWidth, srcHeight, hColorDC, 0, 0, SRCCOPY);

    // 색상 오버레이
    HDC hOverlayDC = CreateCompatibleDC(hDC);
    HBITMAP hOverlayBmp = CreateCompatibleBitmap(hDC, srcWidth, srcHeight);
    HBITMAP hOldOverlay = (HBITMAP)SelectObject(hOverlayDC, hOverlayBmp);

    HBRUSH hBrush = CreateSolidBrush(tintColor);
    RECT rc = { 0, 0, srcWidth, srcHeight };
    FillRect(hOverlayDC, &rc, hBrush);
    DeleteObject(hBrush);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 100;  // 색상 강도
    bf.AlphaFormat = 0;

    AlphaBlend(hTintDC, 0, 0, srcWidth, srcHeight,
        hOverlayDC, 0, 0, srcWidth, srcHeight, bf);

    SelectObject(hOverlayDC, hOldOverlay);
    DeleteObject(hOverlayBmp);
    DeleteDC(hOverlayDC);

    // === 2단계: 알파 마스크 적용 (흰색=보임, 검은색=투명) ===
    HDC hResultDC = CreateCompatibleDC(hDC);
    HBITMAP hResultBmp = CreateCompatibleBitmap(hDC, srcWidth, srcHeight);
    HBITMAP hOldResult = (HBITMAP)SelectObject(hResultDC, hResultBmp);

    // 검은 배경으로 초기화
    HBRUSH hBlackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(hResultDC, &rc, hBlackBrush);

    // 알파 마스크를 이용한 마스킹
    // SRCAND: 알파가 흰색(255)인 부분만 통과
    BitBlt(hResultDC, 0, 0, srcWidth, srcHeight, hAlphaDC, 0, 0, SRCAND);

    // 틴트된 이미지를 마스크 영역에만 합성
    // SRCPAINT: OR 연산으로 합성
    BitBlt(hResultDC, 0, 0, srcWidth, srcHeight, hTintDC, 0, 0, SRCPAINT);

    // === 3단계: 화면에 반투명으로 그리기 ===
    bf.SourceConstantAlpha = 180;  // 전체 투명도
    AlphaBlend(hDC, x, y, width, height,
        hResultDC, 0, 0, srcWidth, srcHeight, bf);

    // 정리
    SelectObject(hResultDC, hOldResult);
    DeleteObject(hResultBmp);
    DeleteDC(hResultDC);

    SelectObject(hTintDC, hOldTint);
    DeleteObject(hTintBmp);
    DeleteDC(hTintDC);
}
