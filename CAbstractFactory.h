#pragma once
#include "CObj.h"
#include "CBuilding.h"
#include "CCommandCenter.h"
#include "CBarracks.h"
#include "CFactory.h"
#include "CStarport.h"
#include "CSupplyDepot.h"

struct CBuildingFactory
{
	static CBuilding* Create(eBuildingType type)
	{
		CBuilding* pBuilding = nullptr;
		switch (type)
		{
		case eBuildingType::COMMAND_CENTER:
			pBuilding = new CCommandCenter;
			break;
		case eBuildingType::BARRACKS:
			pBuilding = new CBarracks;
			break;
		case eBuildingType::FACTORY:
			pBuilding = new CFactory;
			break;
		case eBuildingType::STARPORT:
			pBuilding = new CStarport;
			break;
		case eBuildingType::SUPPLY_DEPOT:
			pBuilding = new CSupplyDepot;
			break;
		default:
			break;
		}
		if (!pBuilding) return nullptr;
		pBuilding->Initialize();
		pBuilding->SetBuildingData();
		return pBuilding;
	}
};

template<typename T>
class CAbstractFactory
{
public:
	CAbstractFactory() {}
	~CAbstractFactory() {}

public:
	static CObj* Create()
	{
		CObj* pObj = new T;
		pObj->Initialize();

		return pObj;
	}

	static CObj* Create(float fX, float fY)
	{
		CObj* pObj = new T;
		pObj->Initialize();
		pObj->Set_Pos(fX, fY);

		return pObj;
	}

	static CObj* Create(float fX, float fY, float fCX, float fCY)
	{
		CObj* pObj = new T;
		pObj->Initialize();
		pObj->Set_Pos(fX, fY);
		pObj->Set_Size(fCX, fCY);

		return pObj;
	}
};

