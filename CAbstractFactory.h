#pragma once
#include "CObj.h"
#include "CBuilding.h"
#include "CCommandCenter.h"
#include "CBarracks.h"
#include "CEngineeringBay.h"
#include "CTurret.h"
#include "CAcademy.h"
#include "CBunker.h"
#include "CFactory.h"
#include "CFactoryAddOn.h"
#include "CStarport.h"
#include "CStarportAddOn.h"
#include "CScienceFacility.h"
#include "CSupplyDepot.h"
#include "CRefinery.h"
#include "CArmory.h"
#include "CCombatStation.h"
#include "CNuclearSilo.h"
#include "CSciencePhysics.h"
#include "CScienceSecret.h"
//프로토스
#include "CNexus.h"
#include "CPylon.h"
#include "CAssimilator.h"
#include "CGateway.h"
#include "CForge.h"
#include "CPhotonCannon.h"
#include "CCyberneticsCore.h"
#include "CShieldBattery.h"
#include "CRoboticsFacility.h"
#include "CStargate.h"
#include "CCitadelOfAdun.h"
#include "CRoboticsSupportBay.h"
#include "CFleetBeacon.h"
#include "CTemplarArchives.h"
#include "CObservatory.h"
#include "CArbiterTribunal.h"
//저그
#include "CHatchery.h"
#include "CSpawningPool.h"
#include "CHydraliskDen.h"
#include "CSpire.h"
#include "CUltraliskDen.h"

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
		case eBuildingType::COMBAT_STATION:
			pBuilding = new CCombatStation;
			break;
		case eBuildingType::NUCLEAR_SILO:
			pBuilding = new CNuclearSilo;
			break;
		case eBuildingType::BARRACKS:
			pBuilding = new CBarracks;
			break;
		case eBuildingType::ENGINEERING_BAY:
			pBuilding = new CEngineeringBay;
			break;
		case eBuildingType::TURRET:
			pBuilding = new CTurret;
			break;
		case eBuildingType::ACADEMY:
			pBuilding = new CAcademy;
			break;
		case eBuildingType::BUNKER:
			pBuilding = new CBunker;
			break;
		case eBuildingType::FACTORY:
			pBuilding = new CFactory;
			break;
		case eBuildingType::FACTORY_ADDON:
			pBuilding = new CFactoryAddOn;
			break;
		case eBuildingType::STARPORT:
			pBuilding = new CStarport;
			break;
		case eBuildingType::STARPORT_ADDON:
			pBuilding = new CStarportAddOn;
			break;
		case eBuildingType::SCIENCE_FACILITY:
			pBuilding = new CScienceFacility;
			break;
		case eBuildingType::SCIENCE_SECRET:
			pBuilding = new CScienceSecret;
			break;
		case eBuildingType::SCIENCE_PHYSICS:
			pBuilding = new CSciencePhysics;
			break;
		case eBuildingType::ARMORY:
			pBuilding = new CArmory;
			break;
		case eBuildingType::SUPPLY_DEPOT:
			pBuilding = new CSupplyDepot;
			break;
		case eBuildingType::REFINERY:
			pBuilding = new CRefinery;
			break;
			//프로토스
		case eBuildingType::NEXUS:
			pBuilding = new CNexus;
			break;
		case eBuildingType::PYLON:
			pBuilding = new CPylon;
			break;
		case eBuildingType::ASSIMILATOR:
			pBuilding = new CAssimilator;
			break;
		case eBuildingType::GATEWAY:
			pBuilding = new CGateway;
			break;
		case eBuildingType::FORGE:
			pBuilding = new CForge;
			break;
		case eBuildingType::PHOTON_CANNON:
			pBuilding = new CPhotonCannon;
			break;
		case eBuildingType::CYBERNETICS_CORE:
			pBuilding = new CCyberneticsCore;
			break;
		case eBuildingType::SHIELD_BATTERY:
			pBuilding = new CShieldBattery;
			break;
		case eBuildingType::ROBOTICS_FACILITY:
			pBuilding = new CRoboticsFacility;
			break;
		case eBuildingType::STARGATE:
			pBuilding = new CStargate;
			break;
		case eBuildingType::CITADEL_OF_ADUN:
			pBuilding = new CCitadelOfAdun;
			break;
		case eBuildingType::ROBOTICS_SUPPORT_BAY:
			pBuilding = new CRoboticsSupportBay;
			break;
		case eBuildingType::FLEET_BEACON:
			pBuilding = new CFleetBeacon;
			break;
		case eBuildingType::TEMPLAR_ARCHIVES:
			pBuilding = new CTemplarArchives;
			break;
		case eBuildingType::OBSERVATORY:
			pBuilding = new CObservatory;
			break;
		case eBuildingType::ARBITER_TRIBUNAL:
			pBuilding = new CArbiterTribunal;
			break;
			//져그
		case eBuildingType::HATCHERY:
			pBuilding = new CHatchery;
			break;
		case eBuildingType::SPAWNING_POOL:
			pBuilding = new CSpawningPool;
			break;
		case eBuildingType::HYDRALISK_DEN:
			pBuilding = new CHydraliskDen;
			break;
		case eBuildingType::SPIRE:
			pBuilding = new CSpire;
			break;
		case eBuildingType::ULTRALISK_DEN:
			pBuilding = new CUltraliskDen;
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

