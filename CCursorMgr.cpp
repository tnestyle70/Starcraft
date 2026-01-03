#include "pch.h"
#include "CCursorMgr.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"
#include "CObjMgr.h"
#include <string>

CCursorMgr* CCursorMgr::m_pInstance = nullptr;

CCursorMgr::CCursorMgr()
	: m_eCursorType(eCursorType::DEFAULT)
	, m_iFrameIndex(0)             
	, m_dwFrameTime(GetTickCount())
	, m_dwFrameDelay(100)           // 100ms마다 프레임 전환
	, m_fRotation(0.f)
	, m_fScale(1.f)
	, m_bClickEffect(false)
	, m_dwClickStart(0)
	, m_iEffectFrame(0)
{
}

CCursorMgr::~CCursorMgr()
{
	Release();
}

void CCursorMgr::Initialize()
{
	// 커서 스프라이트 시트 로드
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Cursor/Cursor2.bmp", L"Cursor");

	// 윈도우 기본 커서 숨기기
	ShowCursor(FALSE);
}

void CCursorMgr::Update()
{
	// 마우스 스크린 좌표 갱신 (카메라 영향 받지 않음!)
	m_vMousePos = CInputMgr::Get_Instance()->GetScreenMouse();

	//커서 타입 업데이트
	UpdateCursorType();

	// 애니메이션 업데이트
	UpdateAnimation();

	if (CInputMgr::Get_Instance()->KeyDown(RIGHT_MOUSE))
	{
		m_bClickEffect = true;
		m_dwClickStart = GetTickCount();
		//m_iEffectFrame = 0;
	}

	// 클릭 이펙트 업데이트
	if (m_bClickEffect)
	{
		DWORD dwCurTime = GetTickCount();
		DWORD dwElapsed = dwCurTime - m_dwClickStart;

		//프레임 갱신
		m_iEffectFrame = (int)(dwElapsed / EFFECT_FRAME_DELAY);

		// 마지막 프레임 이후 종료
		if (m_iEffectFrame >= CLICK_EFFECT_FRAMES)
		{
			m_bClickEffect = false;
			m_iEffectFrame = 0;
		}
	}
}

void CCursorMgr::UpdateCursorType()
{
	//검사는 월드 좌표로!
	Vec2 worlMousePos = CInputMgr::Get_Instance()->GetWorldMouse();
	OBJID hover = CObjMgr::Get_Instance()->CheckHoveredObject(worlMousePos);
	//호버링된 오브젝트가 있을 경우 커서 타입 변경
	switch (hover)
	{
	case OBJID::OBJ_UNIT:
	case OBJID::OBJ_BUILDING:
	{
		m_eCursorType = eCursorType::HOVER_ALLY;
		break;
	}
	case OBJID::OBJ_RESOURCE:
	{
		m_eCursorType = eCursorType::HOVER_RESOURCE;
		break;
	}
	case OBJID::OBJ_ENEMY:
	{
		m_eCursorType = eCursorType::HOVER_ENEMY;
		break;
	}
	case OBJID::OBJ_END:
	{
		m_eCursorType = eCursorType::DEFAULT;
		break;
	}
	default:
		break;
	}
}

void CCursorMgr::UpdateAnimation()
{
	DWORD dwCurTime = GetTickCount();
	// DEFAULT 커서는 애니메이션 (0~5 프레임 순환)
	if (m_eCursorType == eCursorType::DEFAULT)
	{
		if (dwCurTime - m_dwFrameTime > m_dwFrameDelay)
		{
			m_iFrameIndex = (m_iFrameIndex + 1) % DEFAULT_MAX_FRAMES;
			m_dwFrameTime = dwCurTime;
		}
	}
	else if(m_eCursorType == eCursorType::HOVER_ALLY)
	{
		if (dwCurTime - m_dwFrameTime > m_dwFrameDelay)
		{
			m_iFrameIndex = (m_iFrameIndex + 1) % HOVER_MAX_FRAMES;
			m_dwFrameTime = dwCurTime;
		}
	}
	else if (m_eCursorType == eCursorType::HOVER_RESOURCE)
	{
		if (dwCurTime - m_dwFrameTime > m_dwFrameDelay)
		{
			m_iFrameIndex = (m_iFrameIndex + 2) % HOVER_MAX_FRAMES;
			m_dwFrameTime = dwCurTime;
		}
	}
	else if (m_eCursorType == eCursorType::HOVER_ENEMY)
	{
		if (dwCurTime - m_dwFrameTime > m_dwFrameDelay)
		{
			m_iFrameIndex = (m_iFrameIndex + 3) % HOVER_MAX_FRAMES;
			m_dwFrameTime = dwCurTime;
		}
	}
}

void CCursorMgr::Render(HDC hDC)
{
	// 1. 클릭 이펙트 - 간단한 버전
	if (m_bClickEffect)
	{
		HDC hCursorDC = CBmpMgr::Get_Instance()->Find_Image(L"Cursor");
		if (hCursorDC)
		{
			//row 4, col 0~4
			int srcX = m_iEffectFrame * SPRITE_SIZE;
			int srcY = SPRITE_SIZE * 5;
			int iPosX = (int)m_vMousePos.fX - SPRITE_SIZE / 2;
			int iPosY = (int)m_vMousePos.fY - SPRITE_SIZE / 2;

			GdiTransparentBlt(hDC,
				iPosX, iPosY,
				SPRITE_SIZE, SPRITE_SIZE,
				hCursorDC,
				srcX, srcY,
				SPRITE_SIZE, SPRITE_SIZE,
				RGB(255, 255, 255));
		}
	}

	//2. 클릭 이후 커서 
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Cursor");
	if (!hMemDC)
		return;

	// 커서 타입에 따른 스프라이트 위치 계산
	int iCursorIndex = (int)m_eCursorType;

	int iSrcX = 0;
	int iSrcY = 0;

	// Row 0: DEFAULT 커서 애니메이션 (0~5)
	// Row 1~: 다른 커서 타입들
	if (m_eCursorType == eCursorType::DEFAULT)
	{
		// 첫 번째 행, 애니메이션 프레임
		iSrcX = m_iFrameIndex * SPRITE_SIZE;
		iSrcY = 0;
	}
	else if(m_eCursorType == eCursorType::HOVER_ALLY)
	{
		iSrcX = m_iFrameIndex * SPRITE_SIZE;
		iSrcY = SPRITE_SIZE * 2;
	}
	else if (m_eCursorType == eCursorType::HOVER_RESOURCE)
	{
		iSrcX = m_iFrameIndex * SPRITE_SIZE;
		iSrcY = SPRITE_SIZE * 3;
	}
	else if (m_eCursorType == eCursorType::HOVER_ENEMY)
	{
		iSrcX = m_iFrameIndex * SPRITE_SIZE;
		iSrcY = SPRITE_SIZE * 4;
	}
	// 렌더링 크기 (클릭 이펙트 스케일 적용)
	int iRenderSize = (int)(RENDER_SIZE);

	// 마우스 위치 (Vec2가 x,y 멤버를 가지는 경우)
	// 만약 Vec2가 fX, fY를 사용한다면 m_vMousePos.fX로 변경
	int iPosX = (int)m_vMousePos.fX - iRenderSize / 2;
	int iPosY = (int)m_vMousePos.fY - iRenderSize / 2;

	// GdiTransparentBlt로 투명 렌더링 (흰색 배경 제거)
	GdiTransparentBlt(hDC,
		iPosX,
		iPosY,
		iRenderSize,
		iRenderSize,
		hMemDC,
		iSrcX,
		iSrcY,
		SPRITE_SIZE,
		SPRITE_SIZE,
		RGB(255, 255, 255));  // 흰색을 투명하게
}

void CCursorMgr::Release()
{
	// 윈도우 커서 복구
	ShowCursor(TRUE);
}

void CCursorMgr::SetClickEffect()
{
	m_bClickEffect = true;
	m_dwClickStart = GetTickCount();
	m_fScale = 1.f;
}