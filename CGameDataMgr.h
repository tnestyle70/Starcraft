#pragma once
#include "Define.h"

class CGameDataMgr
{
private:
    CGameDataMgr() {};
    ~CGameDataMgr() {};
public:
    // 종족 선택 저장/조회
    void Set_PlayerRace(eRaceType eRace) { m_ePlayerRace = eRace; }
    eRaceType Get_PlayerRace() const { return m_ePlayerRace; }
private:
    eRaceType m_ePlayerRace = eRaceType::RACE_PROTOSS;  // 플레이어가 선택한 종족
public:
    static CGameDataMgr* Get_Instance()
    {
        if (nullptr == m_pInstance)
            m_pInstance = new CGameDataMgr;
        return m_pInstance;
    }

    static void Destroy_Instance()
    {
        if (m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = nullptr;
        }
    }
private:
    static CGameDataMgr* m_pInstance;
};
