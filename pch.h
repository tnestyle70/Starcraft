#ifndef PCH_H
#define PCH_H
// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"

#include <list>
#include <map>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <wingdi.h>  // ← AlphaBlend를 위해 필요
#pragma comment(lib, "msimg32.lib")  // ← AlphaBlend 라이브러리


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
