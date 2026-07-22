#pragma once
#include "CObj.h"

enum eEffectImageType
{
	BMP, PNG
};

enum class eEffectType
{
	COL_BASE, ROW_BASE, DIRECTIONAL
};

class CEffect : public CObj
{
public:
	CEffect();
	virtual ~CEffect();
public:
	void Initialize() override;
	int	 Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override ;
public:
	void Set_Effect(const TCHAR* pKey, int iFrameCount,
		float fCX, float fCY,
		eEffectType type,
		COLORREF transparent,
		float fDelay = 0.05f, bool bLoop = false,
		eEffectImageType imageType = eEffectImageType::BMP);
	void Set_DirectionalEffect(const TCHAR* pKey,
		int iDirectionCount,
		int iFrameCount,
		float fCX, float fCY,
		int iDirection,
		COLORREF transparent,
		float fDelay = 0.05f,
		bool bLoop = false,
		eEffectImageType imageType = eEffectImageType::BMP);
private:
	eEffectImageType m_eImageType;
	const TCHAR* m_pEffectKey; //이펙트 키
	//애니메이션 정보
	int m_iFrameCount;
	int m_iCurrentFrame;
	float m_fFrameDelay;
	float m_fFrameTime = 0.f;
	bool m_bLoop;
	eEffectType m_eEffectType;
	COLORREF m_rgbTransparent;
	//방향 
	int m_iDirection; //현재 방향
	int m_iDirectionCount; //총 방향 수 
};
