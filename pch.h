#ifndef PCH_H
#define PCH_H
// 여기에 미리 컴파일하려는 헤더 추가
// 순서 중요!
#include <Windows.h>
#include <wingdi.h>
#pragma comment(lib, "msimg32.lib")

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

//Video 
#include <Vfw.h>
#pragma comment(lib, "vfw32.lib")

// FMOD (사운드)
#include "fmod.h"
#include "fmod.hpp"
#pragma comment(lib, "fmodex_vc.lib")

#include <io.h>
#include <iostream>

#include <list>
#include <map>
#include <vector>
#include <algorithm>

#include "framework.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 
#endif
#endif

using namespace std;

#endif //PCH_H
