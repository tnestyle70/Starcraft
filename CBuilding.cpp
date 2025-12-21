#include "pch.h"
#include "CBuilding.h"
#include "CTimeMgr.h"
#include "CTileMgr.h"
#include "CScrollMgr.h"

CBuilding::CBuilding() : m_bGhost(false), m_bComplete(false), m_bCanPlace(false),
	m_iHP(0), m_iMaxHP(0), m_fConstructDuration(0.f), m_fConstructElapsed(0.f), m_iWidth(0), m_iHeight(0)
{
	ZeroMemory(&m_eType, sizeof(eBuildingType));
	ZeroMemory(&m_eState, sizeof(eBuildingState));
	ZeroMemory(&m_tCost, sizeof(ResourceCost));
}

CBuilding::~CBuilding()
{
}

void CBuilding::Initialize()
{
	//CBuilding을 초기화할 때 순수 가상함수로 선언되어있는 SetBuildingData를 호출해서
	//건물 정보 초기화 일관되게 진행
	SetBuildingData();
	m_iHP = m_iMaxHP;
	m_eState = eBuildingState::DEPLOY;
}

int CBuilding::Update()
{
	if (m_eState == eBuildingState::DESTROY)
		return DEAD;

	if (m_eState == eBuildingState::DEPLOY)
	{
		m_bCanPlace = CanPlace({ m_tInfo.fX, m_tInfo.fY });
	}
	else if (m_eState == eBuildingState::CONSTRUCT)
	{
		UpdateConstruct();
	}
	else if (m_eState == eBuildingState::COMPLETE)
	{
		//완료 업데이트 진행
	}

	return 0;
}

void CBuilding::Late_Update()
{
}

void CBuilding::Render(HDC hdc)
{
	//TODO CObj 기본 이미지 렌더 호출하기
	if (!m_bGhost) return;

	int row0 = m_iPlaceRow;
	int col0 = m_iPlaceCol;

	// 혹시 SetPlace 안 된 상황 대비
	if (row0 < 0 || col0 < 0)
	{
		if (!CalcSizeTopLeft(Get_Pos(), row0, col0))
			return;
	}

	float scrX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float scrY = CScrollMgr::Get_Instance()->Get_ScrollY();

	RECT rc;
	rc.left = (int)(col0 * TILECX - scrX);
	rc.top = (int)(row0 * TILECY - scrY);
	rc.right = (int)((col0 + m_iWidth) * TILECX - scrX);
	rc.bottom = (int)((row0 + m_iHeight) * TILECY - scrY);

	// 채움(원하면 주석 처리하고 테두리만 남겨도 됨)
	HBRUSH b = CreateSolidBrush(m_bCanPlace ? RGB(0, 200, 0) : RGB(200, 0, 0));
	FillRect(hdc, &rc, b);  // :contentReference[oaicite:1]{index=1}
	DeleteObject(b);

	// 테두리
	HPEN pen = CreatePen(PS_SOLID, 2, m_bCanPlace ? RGB(0, 120, 0) : RGB(120, 0, 0));
	HGDIOBJ oldPen = SelectObject(hdc, pen);
	HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

	Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom); // :contentReference[oaicite:2]{index=2}

	SelectObject(hdc, oldBrush);
	SelectObject(hdc, oldPen);
	DeleteObject(pen);
}

void CBuilding::Release()
{
}

void CBuilding::SetGhost(bool bGhost)
{
	//bDeploy 상태면 배치 상태로 전환
	if (bGhost)
	{
		m_bGhost = true;
		m_bComplete = false;
		m_eState = eBuildingState::DEPLOY;
		return;
	}
	//bDeploy 상태가 아닐 경우 배치 확정 -> 건설 시작 시도
	if (m_eState != eBuildingState::DEPLOY)
		return;
	if (!m_bGhost)
		return;
	
	if (!CResourceMgr::Get_Instance()->TrySpend(m_tCost, false))
		return;

	m_bGhost = false;
	m_eState = eBuildingState::CONSTRUCT;
	m_fConstructElapsed = 0.f;
	m_iHP = 1;
}

bool CBuilding::CanPlace(const Vec2& worldPos)
{
	//타일 점유 빌드 가능 영역 검사 
	CTileMgr* tileMgr = CTileMgr::Get_Instance();

	int row, col;
	if (!CalcSizeTopLeft(worldPos, row, col))
		return false;

	return tileMgr->CanConstruct(row, col, m_iWidth, m_iHeight);
}

void CBuilding::AppplyOccupy()
{
	//배치하고 난 이후 점유 처리
	CTileMgr* tileMgr = CTileMgr::Get_Instance();
	int row0, col0;
	//배치 확정할 때 row, col 저장 
	if (!CalcSizeTopLeft(Get_Pos(), row0, col0)) return;

	for (int r = row0; r < row0 + m_iHeight; ++r)
		for (int c = col0; c < col0 + m_iWidth; ++c)
			tileMgr->SetOccupy(r, c, true);
}

void CBuilding::ReleaseOccupy()
{
	CTileMgr* tileMgr = CTileMgr::Get_Instance();
	int row0, col0;
	//배치 확정할 때 row, col 저장 
	if (!CalcSizeTopLeft(Get_Pos(), row0, col0)) return;

	for (int r = row0; r < row0 + m_iHeight; ++r)
		for (int c = col0; c < col0 + m_iWidth; ++c)
			tileMgr->SetOccupy(r, c, false);
}

bool CBuilding::CalcSizeTopLeft(const Vec2& worldPos, int& outRow, int& outCol) const
{
	CTileMgr* tileMgr = CTileMgr::Get_Instance();

	int row, col;
	if (!tileMgr->WorldToCell(worldPos, row, col)) return false;
	//홀수 짝수 size 대응
	int halfW = (m_iWidth - 1) / 2;
	int halfH = (m_iHeight - 1) / 2;

	outRow = row - halfH;
	outCol = col - halfW;

	return true;
}

void CBuilding::UpdateConstruct()
{
	if (!m_pBuilder) return;

	float fDeltaTime = CTimeMgr::Get_Instance()->GetDT();
	m_fConstructElapsed += fDeltaTime;

	float time = (m_fConstructDuration > 0.f) ?
		(m_fConstructElapsed / m_fConstructDuration) : 1.f;
	if (time > 1.f) time = 1.f;

	m_iHP = (int)(m_iMaxHP * time);
	if (m_iHP < 1) m_iHP = 1;

	if (time >= 1.f)
	{
		m_iHP = m_iMaxHP;
		m_bComplete = true;
		m_eState = eBuildingState::COMPLETE;
		ConstructComplete();
	}
}
