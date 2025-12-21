#define NOMINMAX
#include "pch.h"
#include "CNavMgr.h"
#include "CTileMgr.h"
#include <queue>
#include <string>
#include <sstream>

CNavMgr* CNavMgr::m_pInstance = nullptr;

//휴리스틱 함수 -> 현재 노드에서 목표까지 남은 거리의 추정값을 계산
static float OctileHeuristic(int r0, int c0, int r1, int c1)
{
    int dr = abs(r1 - r0);
    int dc = abs(c1 - c0);
    int mn = min(dr, dc);
    int mx = max(dr, dc);
    // 8방향 기준 휴리스틱(Octile)
    //대각선으로 mm변 + 직선으로 (mx - mn)번 가는 비용 반환
    return 1.41421356f * mn + 1.0f * (mx - mn);
}

void CNavMgr::BuildFromTile()
{
    CTileMgr* tile = CTileMgr::Get_Instance();

    //value - 0로 초기화
    m_vecWalkable.assign(TILEX * TILEY, 0);
    m_vecCost.assign(TILEX * TILEY, 1);

    for (int r = 0; r < TILEY; ++r)
    {
        for (int c = 0; c < TILEX; ++c)
        {
            CObj* pObjTile = CTileMgr::Get_Instance()->GetTile(r, c);
            CTile* pTile = dynamic_cast<CTile*>(pObjTile);
            int iOption = pTile->Get_Option();
            int iCost = pTile->Get_Cost();
            //option - 1 이동불가
            bool bWalkable = (iOption == 0); //이동 가능 0, 이동 불가능 1
            m_vecWalkable[r * TILEX + c] = bWalkable ? 0 : 1;
            m_vecCost[r * TILEX + c] = iCost;
        }
    }
}

bool CNavMgr::IsWalkable(int row, int col) const
{
    if (!InRange(row, col))
        return false;

    if (m_vecWalkable[row * TILEX + col] == 0)
        return true;
    if (m_vecWalkable[row * TILEX + col] == 1)
        return false;
}

bool CNavMgr::InRange(int row, int col) const
{
    //row, col이 타일 사이즈 기준 내부에 존재하는지 판단
    return (row >= 0 && row < 128) && (col >= 0 && col < 128);
}

bool CNavMgr::WorldToCell(Vec2 World, int& outR, int& outC)
{
    outC = floor(World.fX / TILECX);
    outR = floor(World.fY / TILECY);

	return InRange(outR, outC);
}

Vec2 CNavMgr::CellToWolrdCenter(int row, int col)
{
    //cell -> World로 변환
    Vec2 w;
    w.fX = TILECX * col + TILECX * 0.5f;
    w.fY = TILECY * row + TILECY * 0.5f;

    return w;
}

bool CNavMgr::SnapToNearestWalkable(int& ioR, int& ioC, int iMaxRadius)
{
    //goal이 막혔을 경우 근처 walkable 찾기 구현
    if (InRange(ioR, ioC) && IsWalkable(ioR, ioC))
        return true;

    int bestR = -1, bestC = -1;
    int bestDist2 = INT_MAX;

    for (int rad = 1; rad <= iMaxRadius; ++rad)
    {
        // 테두리만 훑는게 효율적이지만, 우선은 안전하게 전체 스캔
        for (int dr = -rad; dr <= rad; ++dr)
        {
            for (int dc = -rad; dc <= rad; ++dc)
            {
                int nr = ioR + dr;
                int nc = ioC + dc;
                if (!InRange(nr, nc)) continue;
                if (!IsWalkable(nr, nc)) continue;

                int d2 = dr * dr + dc * dc;
                if (d2 < bestDist2)
                {
                    bestDist2 = d2;
                    bestR = nr;
                    bestC = nc;
                }
            }
        }

        if (bestR != -1)
        {
            ioR = bestR;
            ioC = bestC;
            return true;
        }
    }

    return false;
}

vector<int> CNavMgr::AStarCells(int scrR, int scrC, int goalR, int goalC)
{
    vector<int> empty;

    if (!InRange(scrR, scrC) || !InRange(goalR, goalC))
        return empty;
    //이웃 자체를 후보에서 제외하기
    if (!IsWalkable(scrR, scrC))
        return empty;

    //goal이 막혔을 경우 스냅
    if (!SnapToNearestWalkable(goalR, goalC, 12))
        return empty;

    int iStartIndex = scrR * TILEX + scrC;
    int iGoalIndex = goalR * TILEX + goalC;

    const float INF = std::numeric_limits<float>::infinity();

    //score, camefrom, closed vector 선언
    vector<float> gScore(128 * 128, INF);
    vector<int> cameFrom(128 * 128, -1);
    vector<int> closed(128 * 128, 0);

    struct OpenNode
    {
        int iIndex; //노드가 어떤 타일/셀인지 판단
        float f; //A*의 우선순위 점수
        float g; //시작점에서 여기까지 온 누적 비용
    };

    struct Compare
    {
        bool operator()(OpenNode& a, OpenNode& b) const
        {
            return a.f > b.f; //최소 힙 반환
        }
    };
    //우선순위 큐
    priority_queue<OpenNode, vector<OpenNode>, Compare> open;

    gScore[iStartIndex] = 0.f;
    open.push({ iStartIndex, OctileHeuristic(scrR, scrC, goalR, goalC), 0.f });

    // 8방향
    const int dr[8] = { -1,  1,  0,  0, -1, -1,  1,  1 };
    const int dc[8] = { 0,  0, -1,  1, -1,  1, -1,  1 };

    while (!open.empty())
    {
        OpenNode cur = open.top();
        open.pop();

        if (closed[cur.iIndex]) continue;
        closed[cur.iIndex] = 1;

        if (cur.iIndex == iGoalIndex)
        {
            //경로 복원 시작
            vector<int> path;
            while (iGoalIndex != -1)
            {
                path.push_back(iGoalIndex);
                iGoalIndex = cameFrom[iGoalIndex];
            }
            reverse(path.begin(), path.end());
            return path;
        }
        int r = cur.iIndex / 128;
        int c = cur.iIndex % 128;

        for (int k = 0; k < 8; ++k)
        {
            int nr = r + dr[k];
            int nc = c + dc[k];
            if (!InRange(nr, nc)) continue;
            if (!IsWalkable(nr, nc)) continue;

            //코너 끼임 방지 - 대각선 이동의 경우 인접 가로, 세로도 통과 가능해야 함
            //대각선 점프 이동 방지
            bool diagnoal = (dr[k] != 0 && dc[k] != 0);
            if (diagnoal)
            {
                if (!IsWalkable(r, nc) || !IsWalkable(nr, c)) continue;
            }

            int nIndex = nr * TILEX + nc;
            if (closed[nIndex]) continue;
            //A에서 현재 칸에서 이웃칸으로 가는 경로를 갱신하는 핵심 구간 
            //대각선 이동 값은 sqrtf(2) PI 아님
            float fBaseMove = diagnoal ? 1.41421 : 1.f;
            float fTileCost = m_vecCost[nIndex];
            float fTentativeG = gScore[cur.iIndex] + fBaseMove * fTileCost;

            if (fTentativeG < gScore[nIndex])
            {
                gScore[nIndex] = fTentativeG;
                cameFrom[nIndex] = cur.iIndex;

                float h = OctileHeuristic(nr, nc, goalR, goalC);
                float f = fTentativeG + h;
                open.push({ nIndex, f, fTentativeG });
            }
        }
    }
	return empty;
}

vector<Vec2> CNavMgr::RequestPathWorld(Vec2& startW, Vec2& goalW)
{
    vector<Vec2> empty;
    //if (!IsReady()) return empty;

    int scrR, scrC, goalR, goalC;
    if (!WorldToCell(startW, scrR, scrC)) return empty;
    if (!WorldToCell(goalW, goalR, goalC)) return empty;

    vector<int> cellPath = AStarCells(scrR, scrC, goalR, goalC);
    if (cellPath.empty()) return empty;
    //지그재그 완화 
    //cell Path = SmoothPathCells(cellPath);

    vector<Vec2> worldPath;
    worldPath.reserve(cellPath.size());

    for (int idx : cellPath)
    {
        int r = idx / 128;
        int c = idx % 128;
        worldPath.push_back(CellToWolrdCenter(r, c));
    }

    // 시작 점이 현재 위치와 거의 같으면 첫 점 제거(출발 시 "한 칸 튐" 방지)
    if (worldPath.size() >= 2)
    {
        Vec2 p0 = worldPath[0];
        float dx = p0.fX - startW.fX;
        float dy = p0.fY - startW.fY;
        float d2 = dx * dx + dy * dy;
        if (d2 < 4.0f * 4.0f)
            worldPath.erase(worldPath.begin());
    }

    return worldPath;
}