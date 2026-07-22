#pragma once
#include "CObj.h"
#include "CResourceMgr.h"
#include "CUnit.h"

enum class eBuildingState
{
	GHOST, CONSTRUCTING, CONSTRUCT, ATTACK, LIFT, FLYING, LANDING, DESTROY
};

//생산큐에 들어간 작업 1개를 의미하는 구조체
struct ProdJob
{
	eCommandID command;
	float totalTime;
	float remainTime;
	int mineral;
	int gas;
	int unitSupply = 1;
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
	virtual void SetBuildingData() PURE;
	virtual void Destroy() PURE;
	virtual int GetRequiredTileValue() const { return 0; } //기본 빈 땅
	virtual void RenderPylonPower(HDC hDC);
public:
	bool StartConstruct(const Vec2& worldPos);
	void UpdateConstructing();
	void RenderProtossConstructAnim(HDC hDC);
	void RenderZergConstructAnim(HDC hDC);
public:
	virtual void CommandCardSlot(vector<CommandSlot>& outSlot);
	virtual void UpdateHotKeys();
	virtual bool ExecuteCommand(eCommandID command, CommandContext& context);
	void SetBuilder(CUnit* pBuilder) { m_pBuilder = pBuilder; }
	//건물 상태 배치, 건설, 완료, 파괴
	void SetGhost(bool bGhost);
	void SetConstruct(bool bConstrct) { m_bConstructing = true; };
	void SetConstructRemain(float remain) { m_fConstructRemain = remain; }
	void SetPlace(int row, int col);
	int GetPlaceRow() { return m_iPlaceRow; }
	int GetPlaceCol() { return m_iPlaceCol; }
	void SetCanPlace(bool bCanPlace) { m_bCanPlace = bCanPlace; }
	bool IsGhost() { return m_bGhost; }
	//배치 가능한지 판단
	bool CanPlace(const Vec2& worldPos);
	//배치 확정 이후 타일 점유
	void AppplyOccupy();
	void ApplyPylonPower();
	void ApplyCrip();
	void ReleaseOccupy();
	//배치 스냅 결과 계산
	bool CalcSizeTopLeft(const Vec2& worldPos, int& outRow, int& outCol) const;
	//건설 시작/진행
	bool IsComplete() { return m_bComplete; }
	//타입/정보 
	eBuildingType GetBuildingType() { return m_eType; }
	ResourceCost& GetCost() { return m_tCost; }
	//건물 Size
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }

	RECT GetWorldRect() const;
	const TCHAR* GetProductionName();
public:
	void SetState(eBuildingState eState) { m_eState = eState; }
	void SetHP(int iHP) { m_iHP = iHP; }
	int Get_HP() { return m_iHP; }
	int Get_MaxHP() { return m_iMaxHP; }
	int Get_Shield() { return m_iShield; }
	int Get_MaxShield() { return m_iMaxShield; }
	bool IsProducing() { return m_bProducing; }
	void SetProducing(bool produce) { m_bProducing = produce; }
	void TakeDamage(int iAttackDamage) override;
	void SetFrameKey(const TCHAR* key) { m_pFrameKey = key; }
	int GetSightRange() { return m_iSightRange; }
	void SetSightRange(int range) { m_iSightRange = range; }
	void RemoveOrdersWithTarget(CObj* pTarget);
public:
	void UpdateBuildingUIInfo();
protected:
	const TCHAR* GetBuildingName();
	const TCHAR* GetIconName_Protoss(eCommandID command);
	int GetIconIndex(eCommandID command);
	virtual void UpdateDestroy();
	virtual void UpdateAnimation();
	virtual void BuildAddOn();
	virtual void PlayCompleteSound();
public:
	void PushOrder(const Order& order) { m_OrderQ.push_back(order); }
	void ClearOrder();
protected:
	virtual void Rally(); //팰리!
	Vec2 m_vRallyPoint;
	bool m_bHasRallyPoint = false;
	virtual void Lift();
	virtual void UpdateMove();
	virtual void Landing();
	float m_fSpeed = 50.f;
	float m_fTargetY;
	bool m_bStartLifting = false;
	bool m_bLifted = false;
	bool m_bStartLanding = false;
protected:
	virtual void UpdateAttack();
	virtual void FireBullet();
	virtual CObj* FindNearestEnemy();
	CObj* m_pTarget = nullptr;
	float m_fAttackRange = 100.f;
	float m_fAttackTimer = 0.f;
	float m_fAttackInterval = 1.f;
	Vec2 m_vDir;
public:
	int DirTo16WayIndex(Vec2& vDir);
	int DirTo32WayIndex(Vec2& vDir);
protected:
	//건물 별로 Green, Red
	TCHAR m_szGreenKey[64];
	TCHAR m_szRedKey[64];
	TCHAR m_szConstructKey[64];
protected:
	//커맨드 카드 상태
	eCommandCardState m_eCommandCardState;
	//명령큐
	std::deque<Order> m_OrderQ;
	//생산큐
	std::deque<ProdJob> m_queue;
	//배치 가능, 건설 여부
	bool m_bGhost;
	bool m_bConstructing = false;
	bool m_bComplete;
	bool m_bCanPlace;
	//생산 여부
	bool m_bProducing = false;
	//SCV 빌더
	CUnit* m_pBuilder;
	eBuildingType m_eType;
	eBuildingState m_eState;
	ResourceCost m_tCost; //미네랄, 가스 정보
	int m_iHP;
	int m_iMaxHP;
	int m_iShield;
	int m_iMaxShield;
	//건설 
	float m_fProgress = 0.f;
	float m_fConstructDuration = 2.f;
	float m_fConstructRemain;
	int m_iConstructFrame = 0;
	//크기
	int m_iWidth;
	int m_iHeight;
	//배치 위치
	int m_iPlaceRow = -1;
	int m_iPlaceCol = -1;
	//시야
	int m_iSightRange;
private:
	void ProtossBmpRender(HDC hdc);
};