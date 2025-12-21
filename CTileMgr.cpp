#include "pch.h"
#include "CTileMgr.h"
#include "CAbstractFactory.h"
#include "CScrollMgr.h"


CTileMgr* CTileMgr::m_pInstance = nullptr;

CTileMgr::CTileMgr()
{
	m_vecTile.reserve(TILEX * TILEY);
}

CTileMgr::~CTileMgr()
{
	Release();
}

void CTileMgr::Initialize()
{
	for (int i = 0; i < TILEY; ++i)
	{
		for (int j = 0; j < TILEX; ++j)
		{
			float fX = float((TILECX >> 1) + (TILECX * j));
			float fY = float((TILECY >> 1) + (TILECY * i));

			CObj* pTile = CAbstractFactory<CTile>::Create(fX, fY);
			m_vecTile.push_back(pTile);
		}
	}
}

void CTileMgr::Update()
{
	for (auto& pTile : m_vecTile)
		pTile->Update();
}

void CTileMgr::Late_Update()
{
	for (auto& pTile : m_vecTile)
		pTile->Late_Update();
}

void CTileMgr::Render(HDC hDC)
{
	int	iCullX = abs((int)CScrollMgr::Get_Instance()->Get_ScrollX() / TILECX);
	int	iCullY = abs((int)CScrollMgr::Get_Instance()->Get_ScrollY() / TILECY);

	int iMaxX = iCullX + WINCX / TILECX + 2;
	int iMaxY = iCullY + WINCY / TILECY + 2;

	for (int i = iCullY; i < iMaxY; ++i)
	{
		for (int j = iCullX; j < iMaxX; ++j)
		{
			int iIndex = i * TILEX + j;

			if (0 > iIndex || (size_t)iIndex >= m_vecTile.size())
				continue;

			m_vecTile[iIndex]->Render(hDC);
		}
	}
}

void CTileMgr::Release()
{
	for_each(m_vecTile.begin(), m_vecTile.end(), Safe_Delete<CObj*>);
	m_vecTile.clear();
}

CObj* CTileMgr::GetTile(int row, int col)
{
	//row, col에 해당하는 CTile을 반환
	return m_vecTile[row * TILEX + col];
}

void CTileMgr::Picking_Tile(POINT pt, int iOption, int iCost)
{
	//마우스가 위치한 타일 x,y
	int		x = pt.x / TILECX;
	int		y = pt.y / TILECY;
	//로직 검토하기
	int iIndex = TILEX * y + x;

	//if (0 > iIndex || (size_t)iIndex >= m_vecTile.size())
	//	return;

	dynamic_cast<CTile*>(m_vecTile[iIndex])->Set_Option(iOption);
	dynamic_cast<CTile*>(m_vecTile[iIndex])->Set_Cost(iCost);
}

void CTileMgr::RenderGrid(HDC hDC, float fScrX, float fScrY)
{
	// 1) 텍스트 투명 배경
	int oldBk = SetBkMode(hDC, TRANSPARENT);
	COLORREF oldColor = SetTextColor(hDC, RGB(0, 255, 0));

	// 2) 폰트(매 프레임 CreateFont 하면 무거우니 static 권장)
	static HFONT hFont = CreateFont(
		14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas"
	);

	HFONT oldF = (HFONT)SelectObject(hDC, hFont);

	// 3) 현재 화면에서 보이는 타일 범위만 계산 (성능 + 깔끔)
	int startCol = fScrX / TILECX;
	int startRow = fScrY / TILECY;

	int endCol = startCol + (WINCX / TILECX) + 2;
	int endRow = startRow + (WINCY / TILECY) + 2;

	// 4) 숫자 찍기 (예: (col,row) 또는 idx)
	wchar_t buf[64];

	for (int r = startRow; r < endRow; ++r)
	{
		for (int c = startCol; c < endCol; ++c)
		{
			// 타일 월드 중심
			int worldCx = c * TILECX + TILECX * 0.5;
			int worldCy = r * TILECY + TILECY * 0.5;

			// 월드 -> 스크린 (스크롤 보정)
			int x = worldCx - fScrX;
			int y = worldCy - fScrY;

			// 화면 밖이면 스킵
			if (x < -TILECX || x > WINCX + TILECX || y < -TILECY || y > WINCY + TILECY)
				continue;

			CObj* pTile = m_vecTile[TILEX * r + c];
			int iOption = dynamic_cast<CTile*>(pTile)->Get_Option();
			// 원하는 표시로 바꿔도 됨:
			wsprintf(buf, L"%d", iOption);
			TextOut(hDC, x - 3, y - 5, buf, lstrlen(buf));
		}
	}
	//격자 그리드 그리기
	//월드 기준 타일 범위
	float fStartCol = fScrX / TILECX;
	float fEndCol = (fScrX + WINCX) / TILECX + 1;
	float fStartRow = fScrY / TILECY;
	float fEndRow = (fScrY + WINCY) / TILECY + 1;

	// 펜 설정(격자색)
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	HPEN oldP = (HPEN)SelectObject(hDC, pen);
	//세로선
	for (int c = fStartCol; c <= fEndCol; ++c)
	{
		int xWorld = c * TILECX;
		int xScr = xWorld - fScrX; // 월드->스크린

		MoveToEx(hDC, xScr, 0, nullptr);           // 시작점 :contentReference[oaicite:2]{index=2}
		LineTo(hDC, xScr, WINCY);                // 끝점 :contentReference[oaicite:3]{index=3}
	}
	// 가로선
	for (int r = fStartRow; r <= fEndRow; ++r)
	{
		int yWorld = r * TILECY;
		int yScr = yWorld - fScrY;

		MoveToEx(hDC, 0, yScr, nullptr);
		LineTo(hDC, WINCX, yScr);
	}
	SelectObject(hDC, oldP);
	DeleteObject(pen);
}

bool CTileMgr::IsOccupy(int row, int col)
{
	if (!InRange(row, col)) return true;
	return m_vecOccupy[row * TILEX + col] != 0;

	return false;
}

void CTileMgr::SetOccupy(int row, int col, bool occupy)
{
	if (!InRange(row, col)) return;
	if ((int)m_vecOccupy.size() != TILEX * TILEY)
	{
		m_vecOccupy.assign(TILEX * TILEY, 0);
	}

	m_vecOccupy[row * TILEX + col] = occupy ? 1 : 0;
}

bool CTileMgr::InRange(int& row, int& col) const
{
	return (row >= 0 && row < TILEY &&
		col >= 0 && col < TILEX);
}

bool CTileMgr::IsBuildableTile(int row, int col)
{
	if (!InRange(row, col)) return false;
	CObj* pObjTile = GetTile(row, col);
	CTile* pTile = dynamic_cast<CTile*>(pObjTile);
	if (!pTile) return false;

	int iOption = pTile->Get_Option();
	return (iOption == 0);
}

bool CTileMgr::CanConstruct(int row, int col, int width, int height)
{
	for (int r = row; r < row + height; ++r)
	{
		for (int c = col; c < col + width; ++c)
		{
			if (!InRange(row, col)) return false;
			if (!IsBuildableTile(row, col)) return false;
			if (IsOccupy(row, col)) return false;
		}
	}
	return true;
}

bool CTileMgr::WorldToCell(const Vec2& world, int& outRow, int& outCol) const
{
	if (world.fX < 0.f || world.fY < 0.f) return false;
	outRow = (int)(world.fY / TILECY);
	outCol = (int)(world.fX / TILECX);
	return InRange(outRow, outCol);
}

Vec2 CTileMgr::CellToWorldCenter(int row, int col) const
{
	return Vec2{col * (float)TILECX + TILECX * 0.5f, 
	row * (float)TILECY + TILECY * 0.5f};
}

Vec2 CTileMgr::CellToWorldTopLeft(int row, int col) const
{
	return Vec2{col * (float)TILECX, row * (float)TILECY};
}

void CTileMgr::Save_Tile()
{
	HANDLE		hFile = CreateFile(L"../Data/Tile.dat",	// 파일 경로
		GENERIC_WRITE,		// 파일 접근 모드 (GENERIC_READ : 읽기 전용)
		NULL,				// 공유 방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈 할 때 허가할 것인가
		NULL,				// 보안 속성, NULL 인 경우 기본값
		CREATE_ALWAYS,		// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL,	// 파일 속성(아무런 속성이 없는 일반 파일 생성)
		NULL);				// 생성할 파일의 속성을 제공할 템플릿

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"Save File Open", _T("FAIL"), MB_OK);
		return;
	}

	int			iOption(0), iCost(0);
	DWORD		dwByte(0);
	//디버깅 
	int cnt = 0, opt0 = 0, opt1 = 0, optOther = 0;

	for (auto& pTile : m_vecTile)
	{
		iOption = dynamic_cast<CTile*>(pTile)->Get_Option();
		iCost = dynamic_cast<CTile*>(pTile)->Get_Cost();
		INFO tTile = pTile->Get_Info();

		if (iOption == 0) opt0++;
		else if (iOption == 1) opt1++;
		else optOther++;

		WriteFile(hFile, &iOption, sizeof(int), &dwByte, nullptr);
		WriteFile(hFile, &iCost, sizeof(int), &dwByte, nullptr);
		WriteFile(hFile, &tTile, sizeof(INFO), &dwByte, nullptr);

		cnt++;
	}

	CloseHandle(hFile);

	wchar_t buf[128];
	swprintf_s(buf, L"SAVE TOTAL=%d\nopt0=%d opt1=%d other=%d", cnt, opt0, opt1, optOther);
	MessageBox(g_hWnd, buf, L"Save Debug", MB_OK);

	//MessageBox(g_hWnd, L"Tile Save 완료", _T("성공"), MB_OK);
}

void CTileMgr::Load_Tile()
{
	HANDLE		hFile = CreateFile(L"../Data/Tile.dat",	// 파일 경로
		GENERIC_READ,		// 파일 접근 모드 (GENERIC_READ : 읽기 전용)
		NULL,				// 공유 방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈 할 때 허가할 것인가
		NULL,				// 보안 속성, NULL 인 경우 기본값
		OPEN_EXISTING,		// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL,	// 파일 속성(아무런 속성이 없는 일반 파일 생성)
		NULL);				// 생성할 파일의 속성을 제공할 템플릿

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"Load File Open", _T("FAIL"), MB_OK);
		return;
	}

	Release();
	//불러올 때 reserve
	m_vecTile.reserve(TILEX * TILEY);

	int			iOption(0), iCost(0);
	DWORD		dwByte(0);
	INFO		tTile{};
	//디버깅
	int cnt = 0, cntOpt1 = 0;
	while (true)
	{
		DWORD br1 = 0, br2 = 0, br3 = 0;
		BOOL ok1 = ReadFile(hFile, &iOption, sizeof(int), &br1, nullptr);
		BOOL ok2 = ReadFile(hFile, &iCost, sizeof(int), &br2, nullptr);
		BOOL ok3 = ReadFile(hFile, &tTile, sizeof(INFO), &br3, nullptr);

		if (!ok1 || !ok2 || !ok3 ||
			br1 != sizeof(int) || br2 != sizeof(int) || br3 != sizeof(INFO))
			break;
		if (iOption == 1) cntOpt1++;
		cnt++;
		//if (dwByte == 0)
		//	break;

		CObj* pTile = CAbstractFactory<CTile>::Create(tTile.fX, tTile.fY);
		dynamic_cast<CTile*>(pTile)->Set_Option(iOption);
		dynamic_cast<CTile*>(pTile)->Set_Cost(iCost);

		m_vecTile.push_back(pTile);
	}

	CloseHandle(hFile);
	wchar_t buf[128];
	swprintf_s(buf, L"TOTAL=%d\nopt1=%d", cnt, cntOpt1);

	MessageBox(g_hWnd, buf, L"Tile Load 완료", MB_OK);
}
