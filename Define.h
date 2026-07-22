#pragma once
#define WINCX 800
#define WINCY 600
#define WORLD_W 4096
#define WORLD_H 4096
#define PURE = 0
#define NOEVENT 0
#define DEAD 1
#define PI 3.141592
#define TILEX 128
#define TILEY 128
#define TILECX 32
#define TILECY 32

constexpr int EDGE_MARGIN = 20;
constexpr float SCROLL_SPEED = 900.f;

extern HWND g_hWnd;

enum OBJID 
{
	OBJ_UI, 
	OBJ_UNIT, 
	OBJ_BUILDING,
	OBJ_ENEMY, 
	OBJ_RESOURCE,
	OBJ_PROJECTILE,
	OBJ_END
};

enum RENDERID{RENDER_BG, RENDER_WORLD, RENDER_UI, RENDER_UI_TOP, RENDER_END};

enum class eBuildingType
{
	NONE,
	COMMAND_CENTER, 
	COMBAT_STATION,
	NUCLEAR_SILO,
	BARRACKS,
	ENGINEERING_BAY,
	TURRET,
	ACADEMY,
	BUNKER,
	FACTORY,
	FACTORY_ADDON,
	REFINERY,
	STARPORT,
	STARPORT_ADDON,
	SCIENCE_FACILITY,
	SCIENCE_SECRET,
	SCIENCE_PHYSICS,
	ARMORY,
	SUPPLY_DEPOT,
	//저그
	HIVE,
	HATCHERY,
	LAIR,
	HYDRALISK_DEN,
	SPIRE,
	SPAWNING_POOL,
	ULTRALISK_DEN,
	//프로토스
	NEXUS,
	PYLON,
	ASSIMILATOR,
	GATEWAY,
	FORGE, 
	PHOTON_CANNON,
	CYBERNETICS_CORE,
	SHIELD_BATTERY,
	ROBOTICS_FACILITY,
	STARGATE,
	CITADEL_OF_ADUN,
	ROBOTICS_SUPPORT_BAY,
	FLEET_BEACON,
	TEMPLAR_ARCHIVES,
	OBSERVATORY,
	ARBITER_TRIBUNAL
};

enum class eUnitType
{
	NONE,
	SCV,
	MARINE, 
	MEDIC, 
	FIREBAT,
	GHOST,
	VULTURE,
	TANK,
	SIEGE_TANK,
	GOLIATH,
	BATTLECRUISER,
	//적 유닛 - 저그
	LAVA,
	ZERGLING,
	HYDRALISK,
	ULTRALISK,
	MUTALISK,
	OVERLOAD,
	DRONE,
	//적 유닛 - 프로토스
	PROBE,
	ZEALOT,
	SHUTTLE,
	ARCHON,
	DARK_ARCHON,
	DARK_TEMPLAR,
	DRAGON,
	ARBITER,
	CARRIER,
	INTERCEPTOR,
	CORSAIR,
	HIGH_TEMPLAR,
	REAVOR,
	SCOUT,
	OBSERVER
};

enum class eCursorType
{
	DEFAULT = 0, //기본 커서
	HOVER_ALLY,
	HOVER_RESOURCE, 
	HOVER_ENEMY,
	MOVE, 
};

enum CHANNELID 
{ 
	SOUND_BGM,
	SOUND_EFFECT,
	SOUND_BUTTON,
	SOUND_UI,
	SOUND_WORLD,
	MAXCHANNEL 
};

enum class eRaceType
{
	RACE_TERRAN, RACE_ZERG, RACE_PROTOSS
};

struct Vec2
{
	float fX, fY;
	Vec2() : fX(0), fY(0) {};
	Vec2(float _fX, float _fY) : fX(_fX), fY(_fY) {};

	Vec2 operator+(const Vec2& rhs) { return { fX + rhs.fX, fY + rhs.fY }; }
};

//Info
typedef struct tagInfo
{
	float fX, fY;
	float fCX, fCY;
}INFO;

//Sate_Delete
template<typename T>
void Safe_Delete(T& p)
{
	if (p)
	{
		OutputDebugString(L"Deleting object\n");
		delete p;
		p = nullptr;
	}
	else
	{
		OutputDebugString(L"Warning: Already deleted!\n");
	}
}

//BitMap 이름 찾기
struct tagFinder
{
	const TCHAR* m_pTag;
	//생성자
	tagFinder(const TCHAR* pTag) : m_pTag(pTag) {};
	template<typename T>
	//매개변수로 들어온 const TCHAR와 map의 key 값이 같은지, 같지 않은지 반환
	bool operator()(T& Pair)
	{
		return !lstrcmp(Pair.first, m_pTag);
	}
};

struct DeleteObj
{
	template<typename T>
	void operator()(T& p)
	{
		if (p)
		{
			delete p;
			p = nullptr;
		}
	}
};

struct DeleteMap
{
	template<typename T>
	void operator()(T& Pair)
	{
		if (Pair.second)
		{
			delete Pair.second;
			Pair.second = nullptr;
		}
	}
};

typedef struct tagFrame
{
	int iFrame; //현재 프레임
	int iStart; //시작 프레임
	int iEnd;
	int iCol; //현재 열
	DWORD dwSpeed;
	DWORD dwTime;
}FRAME;