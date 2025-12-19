#include "pch.h"
#include "CCommandMgr.h"
#include "CSelectionMgr.h"

CCommandMgr* CCommandMgr::m_pInstance = nullptr;

CCommandMgr::CCommandMgr()
{
}

CCommandMgr::~CCommandMgr()
{
}

void CCommandMgr::IssueMove(Vec2& vTargetMouse)
{
	auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
	if (selected.empty()) return;

	//여러 유닛이면 살짝 퍼지는 느낌 주기
    const int count = (int)selected.size();
    const int iCols = (int)ceilf(sqrtf((float)count));
    const int iRows = (count + iCols - 1) / iCols;

    const float fSpacing = 32.f;

    const float cx = (iCols - 1) * 0.5f; // 중심(반칸 포함)
    const float cy = (iRows - 1) * 0.5f;

    for (int i = 0; i < count; ++i)
    {
        CUnit* pUnit = selected[i];
        if (!pUnit || pUnit->IsDead()) continue;

        const int iRow = i / iCols;
        const int iCol = i % iCols;

        Vec2 offset{
            (iCol - cx) * fSpacing,
            (iRow - cy) * fSpacing
        };

        Vec2 dst = vTargetMouse + offset;

        pUnit->ClearOrders();
        pUnit->PushOrder({ eOrderType::MOVE, dst, nullptr });
    }
}
