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

enum OBJID {OBJ_UI, OBJ_UNIT, OBJ_BUILDING ,OBJ_END};

enum RENDERID{RENDER_BG, RENDER_WORLD, RENDER_UI, RENDER_UI_TOP, RENDER_END};

namespace AtlasKey
{
	inline constexpr const wchar_t* MENU = L"UI_MENU";
	inline constexpr const wchar_t* INGAME = L"UI_INGAME";
	inline constexpr const wchar_t* UNIT = L"PROTOSS_UNIT";
}

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
		delete p;
		p = nullptr;
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