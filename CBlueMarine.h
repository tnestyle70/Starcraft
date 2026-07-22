#pragma once
#include "CMarine.h"

// 적 AI 마린 - 자동으로 적을 탐지하고 공격/추격
class CBlueMarine : public CMarine
{
public:
	CBlueMarine();
	virtual ~CBlueMarine();

public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
public:
	void CommandCardSlot(vector<CommandSlot>& outSlot);
private:
	// AI 관련 함수들
	void UpdateAI();							// AI 메인 로직
	CObj* FindNearestEnemy();					// 가장 가까운 적 찾기
	bool IsInAttackRange(CObj* pTarget);		// 사거리 내 체크
	bool HasActiveOrder();						// 현재 명령 실행 중인지
	void IssueAutoAttack(CObj* pTarget);		// 자동 공격 명령
	void IssueAutoMove(CObj* pTarget);			// 자동 추격 명령

private:
	CObj* m_pCurrentTarget;		// 현재 추적 중인 타겟
	float m_fAIUpdateTimer;		// AI 업데이트 주기 타이머
	float m_fAIUpdateInterval;	// AI 업데이트 간격 (0.5초마다 체크)
};
