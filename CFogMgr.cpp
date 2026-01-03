#pragma comment(lib, "Msimg32.lib")
#include "pch.h"
#include "CFogMgr.h"
#include "CTileMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CUnit.h"
#include "CBuilding.h"
#include "CTimeMgr.h"

CFogMgr* CFogMgr::m_pInstance = nullptr;

CFogMgr::CFogMgr() : m_hFogDC(nullptr), m_hFogBitmap(nullptr), m_hOldBitmap(nullptr)
{
}

CFogMgr::~CFogMgr()
{
	Release();
}

void CFogMgr::Initialize()
{
	m_fFogUpdateRate = 0.f;
	m_bFogDirty = true;
	//타일 상태 UNKNOWN으로 초기화 
	m_vecFogState.assign(TILEX * TILEY, eFogState::UNKNOWN);
	//안개 렌더링용 DC 생성
	HDC hDC = GetDC(g_hWnd);
	m_hFogDC = CreateCompatibleDC(hDC);
	m_hFogBitmap = CreateCompatibleBitmap(hDC, WINCX, WINCY);
	m_hOldBitmap = (HBITMAP)SelectObject(m_hFogDC, m_hFogBitmap);
	// 캐시용 DC 생성
	m_hFogCacheDC = CreateCompatibleDC(hDC);
	m_hFogCacheBitmap = CreateCompatibleBitmap(hDC, WINCX, WINCY);
	m_hFogCacheOld = (HBITMAP)SelectObject(m_hFogCacheDC, m_hFogCacheBitmap);
	//ReleaseDC(g_hWnd, hDC);
	// 브러시/펜 미리 생성 (재사용)
	m_hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
	m_hGrayBrush = CreateSolidBrush(RGB(50, 50, 50));
	m_hNullPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));


	//알파 소스
	BITMAPINFO bmi{};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 1;
	bmi.bmiHeader.biHeight = -1; // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* pBits = nullptr;

	m_hAlphaSrcDC = CreateCompatibleDC(hDC);
	m_hAlphaSrcBmp = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
	m_hAlphaSrcOld = (HBITMAP)SelectObject(m_hAlphaSrcDC, m_hAlphaSrcBmp);

	// 1픽셀에 "검정 + 알파" 세팅 (검정이므로 RGB=0, 알파만 의미 있음)
	*(DWORD*)pBits = (DWORD(m_byExploredAlpha) << 24);

	ReleaseDC(g_hWnd, hDC);
}

void CFogMgr::Update()
{
	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_fFogUpdateRate += dt;
	if (m_fFogUpdateRate >= UPDATE_INTERVAL)
	{
		m_fFogUpdateRate = 0.f;
		//이전 프레임의 Visible을 explored로 변경
		ResetVisibleToExplored();
		//현재 아군 유닛 / 건물의 시야로 visible 업데이트
		UpdateVision();
	}
}

void CFogMgr::Render(HDC hDC)
{
	RenderFog(hDC);
}

eFogState CFogMgr::GetFogState(int row, int col)
{
	if (!InRange(row, col))
	{
		return eFogState::UNKNOWN;
	}
	int index = row * TILEX + col;
	return m_vecFogState[index];
}

void CFogMgr::SetFogState(int row, int col, eFogState state)
{
	if (!InRange(row, col))
		return;
	int index = row * TILEX + col;
	m_vecFogState[index] = state;
}

void CFogMgr::UpdateVision()
{
	//아군 유닛, 건물들의 시야 업데이트
	auto& unitList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_UNIT);
	for (auto& pObj : unitList)
	{
		if (pObj->IsDead())
			continue;
		CUnit* pUnit = dynamic_cast<CUnit*>(pObj);
		if (!pUnit)
			continue;
		//아군 유닛의 경우에만 시야 제공 

		//유닛의 위치를 타일 좌표로 변환
		Vec2 pos = pUnit->Get_Pos();
		int row, col;
		if (CTileMgr::Get_Instance()->WorldToCell(pos, row, col))
		{
			int sightRange = pUnit->GetSightRange();
			RevealArea(row, col, sightRange);
		}
	}
	//아군 건물들의 시야 업데이트
	auto& buildingList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_BUILDING);
	for (auto& pObj : buildingList)
	{
		if (pObj->IsDead())
			continue;
		CBuilding* pBuilding = dynamic_cast<CBuilding*>(pObj);
		if (!pBuilding)
			continue;
		//건설 중이거나 고스트 상태면 시야 제공 X
		if (!pBuilding->IsComplete() || pBuilding->IsGhost())
			continue;
		//아군 건물만 시야 제공
		//건물 위치를 타일 좌표로 변환
		Vec2 pos = pBuilding->Get_Pos();
		int row, col;
		if (CTileMgr::Get_Instance()->WorldToCell(pos, row, col))
		{
			int sightRange = pBuilding->GetSightRange();
			RevealArea(row, col, sightRange);
		}
	}
}

void CFogMgr::RevealArea(int centerRow, int centerCol, int sightRange)
{
	bool changed = false;

	//원형 시야 거리 계산 후 적용
	for (int r = centerRow - sightRange; r <= centerRow + sightRange; ++r)
	{
		for (int c = centerCol - sightRange; c <= centerCol + sightRange; ++c)
		{
			if (!InRange(r, c))
				continue;
			//유클리드 거리로 원형 시야 계산
			int dr = r - centerRow;
			int dc = c - centerCol;
			float dist = sqrtf((float)(dr * dr + dc * dc));
			if (dist <= sightRange)
			{
				int index = r * TILEX + c;
				//eFogState oldState = m_vecFogState[index];
				////UNKNOWN이면 EXPLORED로 시야 안이므로 VISIBLE로
				//if (m_vecFogState[index] == eFogState::UNKNOWN)
				//{
				//	m_vecFogState[index] = eFogState::VISIBLE;
				//	changed = true;
				//}
				//시야 안이므로 VISIBLE 
				if (m_vecFogState[index] != eFogState::VISIBLE)
				{
					m_vecFogState[index] = eFogState::VISIBLE;
					changed = true;
				}
					
			}
		}
	}
	if (changed)
		m_bFogDirty = true;
}

void CFogMgr::ResetVisibleToExplored()
{
	bool changed = false;

	//모든 Visible 타일들을 explored로 변경
	for (size_t i = 0; i < m_vecFogState.size(); ++i)
	{
		if (m_vecFogState[i] == eFogState::VISIBLE)
		{
			m_vecFogState[i] = eFogState::EXPLORED;
			changed = true;
		}
	}
	if (changed)
		m_bFogDirty = true;
}

void CFogMgr::RenderFog(HDC hDC)
{
	float scrX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float scrY = CScrollMgr::Get_Instance()->Get_ScrollY();

	int startCol = (int)(scrX / TILECX);
	int startRow = (int)(scrY / TILECY);
	int endCol = startCol + (WINCX / TILECX) + 2;
	int endRow = startRow + (WINCY / TILECY) + 2;

	HPEN oldPen = (HPEN)SelectObject(hDC, m_hNullPen);

	BLENDFUNCTION bf{};
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;

	for (int r = startRow; r < endRow; ++r)
	{
		for (int c = startCol; c < endCol; ++c)
		{
			if (!InRange(r, c)) continue;

			eFogState state = GetFogState(r, c);
			if (state == eFogState::VISIBLE) continue;

			int screenX = (c * TILECX) - (int)scrX;
			int screenY = (r * TILECY) - (int)scrY;

			if (state == eFogState::UNKNOWN)
			{
				// 완전 검정(불투명)
				HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, m_hBlackBrush);
				Rectangle(hDC, screenX, screenY, screenX + TILECX + 1, screenY + TILECY + 1);
				SelectObject(hDC, oldBrush);
			}
			else if (state == eFogState::EXPLORED)
			{
				// 반투명 검정 오버레이
				AlphaBlend(
					hDC,
					screenX, screenY, TILECX, TILECY,
					m_hAlphaSrcDC,
					0, 0, 1, 1,
					bf
				);
			}
		}
	}

	SelectObject(hDC, oldPen);


	/*

	//안개가 변경되지 않았으면 캐시 사용
	if (!m_bFogDirty)
	{
		BitBlt(hDC, 0, 0, WINCX, WINCY, m_hFogCacheDC, 0, 0, SRCAND);
		return;
	}
	// 캐시 DC를 투명하게 초기화
	BitBlt(m_hFogCacheDC, 0, 0, WINCX, WINCY, hDC, 0, 0, SRCCOPY);

	//m_bFogDirty = false; //Dirty 플래그 

	float scrX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float scrY = CScrollMgr::Get_Instance()->Get_ScrollY();
	//화면에 보이는 타일 범위만 랜더링
	int startCol = (int)(scrX / TILECX);
	int startRow = (int)(scrY / TILECY);
	int endCol = startCol + (WINCX / TILECX) + 2;
	int endRow = startRow + (WINCY / TILECY) + 2;
	//펜 한번만 설정
	HPEN hOldPen = (HPEN)SelectObject(m_hFogCacheDC, m_hNullPen);
	HBRUSH hOldBrush = nullptr;
	for (int r = startRow; r < endRow; ++r)
	{
		for (int c = startCol; c < endCol; ++c)
		{
			if (!InRange(r, c))
				continue;
			eFogState state = GetFogState(r, c);
			//Visible은 렌더링 X
			if (state == eFogState::VISIBLE)
				continue;
			//타일 월드 좌표
			int worldX = c * TILECX;
			int worldY = r * TILECY;
			//스크린 좌표로 변환
			int screenX = worldX - int(scrX);
			int screenY = worldY - int(scrY);

			//색상 결정
			COLORREF fogColor;
			if (state == eFogState::UNKNOWN)
				fogColor = RGB(0, 0, 0); //완전 검은색
			else if (state == eFogState::EXPLORED)
				fogColor = RGB(50, 50, 50);//어두운 회색
			else
				continue;
			// 브러시 생성
			HBRUSH hBrush = CreateSolidBrush(fogColor);
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

			// 펜 없이 사각형 그리기
			HPEN hPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
			HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

			Rectangle(hDC, screenX, screenY, screenX + TILECX + 1, screenY + TILECY + 1);

			// 정리
			SelectObject(hDC, hOldPen);
			SelectObject(hDC, hOldBrush);
			DeleteObject(hPen);
			DeleteObject(hBrush);
		}
	}
	*/
}

void CFogMgr::Release()
{
	// 알파 소스 정리
	if (m_hAlphaSrcDC)
	{
		SelectObject(m_hAlphaSrcDC, m_hAlphaSrcOld);
		DeleteObject(m_hAlphaSrcBmp);
		DeleteDC(m_hAlphaSrcDC);
		m_hAlphaSrcDC = nullptr;
		m_hAlphaSrcBmp = nullptr;
		m_hAlphaSrcOld = nullptr;
	}

	if (m_hBlackBrush) { DeleteObject(m_hBlackBrush); m_hBlackBrush = nullptr; }
	if (m_hGrayBrush) { DeleteObject(m_hGrayBrush);  m_hGrayBrush = nullptr; }
	if (m_hNullPen) { DeleteObject(m_hNullPen);    m_hNullPen = nullptr; }


	if (m_hFogDC)
	{
		SelectObject(m_hFogDC, m_hOldBitmap);
		DeleteObject(m_hFogBitmap);
		DeleteDC(m_hFogDC);
		m_hFogDC = nullptr;
	}

	m_vecFogState.clear();
}


bool CFogMgr::IsVisible(int row, int col)
{
	return GetFogState(row, col) == eFogState::VISIBLE;
}

bool CFogMgr::IsExplored(int row, int col)
{
	eFogState state = GetFogState(row, col);
	return state == eFogState::EXPLORED; //|| state == eFogState::VISIBLE;
}

bool CFogMgr::IsUnknown(int row, int col)
{
	return GetFogState(row, col) == eFogState::UNKNOWN;
}

bool CFogMgr::InRange(int row, int col) const
{
	return (row >= 0 && row < TILEY && col >= 0 && col < TILEX);
}
