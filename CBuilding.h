#pragma once
#include "CObj.h"
#include "CResourceMgr.h"
#include "CUnit.h"

enum class eBuildingType
{
	COMMAND_CENTER, BARRACK, FACTORY, STARPORT
};

enum class eBuildingState
{
	DEPLOY, CONSTRUCT, COMPLETE, DESTROY
};
//생산큐에 들어간 작업 1개를 의미하는 구조체
struct ProdJob
{
	eCommandID command;
	float totalTime;
	float remainTime;
	int mineral;
	int gas;
};

//커맨드 버튼 핫키

class CBuilding : public CObj
{
public:
	CBuilding();
	virtual ~CBuilding();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
public:
	void SetBuilder(CUnit* pBuilder) { m_pBuilder = pBuilder; }
	//건물 상태 배치, 건설, 완료, 파괴
	void SetGhost(bool bGhost);
	void SetPlace(int row, int col) { m_iPlaceRow = row, m_iPlaceCol = col; }
	int GetPlaceRow() { return m_iPlaceRow; }
	int GetPlaceCol() { return m_iPlaceCol; }
	void SetCanPlace(bool bCanPlace) { m_bCanPlace = bCanPlace; }
	bool IsGhost() { return m_bGhost; }
	//배치 가능한지 판단
	bool CanPlace(const Vec2& worldPos);
	//배치 확정 이후 타일 점유
	void AppplyOccupy();
	void ReleaseOccupy();
	//배치 스냅 결과 계산
	bool CalcSizeTopLeft(const Vec2& worldPos, int& outRow, int& outCol) const;
	//건설 시작/진행
	void UpdateConstruct();
	bool IsComplete() { return m_bComplete; }
	//타입/정보 
	eBuildingType GetBuildingType() { return m_eType; }
	ResourceCost& GetCost() { return m_tCost; }
	//건물 Size
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }

	RECT GetWorldRect() const;
protected:
	virtual void SetBuildingData() PURE; 
	virtual void ConstructComplete() PURE;
	virtual void Destroy() PURE;
protected:
	//배치 가능, 건설 여부
	bool m_bGhost;
	bool m_bComplete;
	bool m_bCanPlace;
	//SCV 빌더
	CUnit* m_pBuilder;
	eBuildingType m_eType;
	eBuildingState m_eState;
	ResourceCost m_tCost; //미네랄, 가스 정보
	int m_iHP;
	int m_iMaxHP;
	float m_fConstructDuration;
	float m_fConstructElapsed;
	//크기
	int m_iWidth;
	int m_iHeight;
	//배치 위치
	int m_iPlaceRow = -1;
	int m_iPlaceCol = -1;
};