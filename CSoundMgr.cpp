#include "pch.h"
#include "CSoundMgr.h"

CSoundMgr* CSoundMgr::m_pInstance = nullptr;

CSoundMgr::CSoundMgr()
{
	m_pSystem = nullptr;
}

CSoundMgr::~CSoundMgr()
{
	Release();
}

void CSoundMgr::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD_System_Create(&m_pSystem);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, 1024, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile();
}
void CSoundMgr::Release()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

void CSoundMgr::PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID]);
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayEffect(const TCHAR* pSoundKey, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_CHANNEL* pChannel = nullptr;

	// FMOD_CHANNEL_FREE를 사용하여 자동으로 빈 채널 할당
	// 마지막 파라미터를 nullptr로 하면 채널 포인터를 저장하지 않음
	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &pChannel);
	FMOD_Channel_SetVolume(pChannel, fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::LoadSoundFile()
{

	LoadSoundFileRecursive("../Sound/", "");
	FMOD_System_Update(m_pSystem);

	/*

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	long handle = _findfirst("../Sound/*.*", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "../Sound/";
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);

		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath,
			FMOD_HARDWARE, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.insert({ pSoundKey, pSound });
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 
		// 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	FMOD_System_Update(m_pSystem);

	_findclose(handle);


	*/
}

void CSoundMgr::LoadSoundFileRecursive(const char* szFolderPath, const char* szRelativePath)
{
	_finddata_t fd;

	char szSearchPath[256];
	sprintf_s(szSearchPath, "%s*.*", szFolderPath);

	long handle = _findfirst(szSearchPath, &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	while (iResult != -1)
	{
		// "." 와 ".." 폴더는 스킵
		if (strcmp(fd.name, ".") != 0 && strcmp(fd.name, "..") != 0)
		{
			char szFullPath[256];
			sprintf_s(szFullPath, "%s%s", szFolderPath, fd.name);

			// 디렉토리인 경우 재귀 호출
			if (fd.attrib & _A_SUBDIR)
			{
				char szNewFolder[256];
				char szNewRelative[256];

				sprintf_s(szNewFolder, "%s%s/", szFolderPath, fd.name);
				sprintf_s(szNewRelative, "%s%s/", szRelativePath, fd.name);

				LoadSoundFileRecursive(szNewFolder, szNewRelative);
			}
			// 파일인 경우 사운드로 로드
			else
			{
				FMOD_SOUND* pSound = nullptr;
				FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath,
					FMOD_HARDWARE, 0, &pSound);

				if (eRes == FMOD_OK)
				{
					// 상대 경로 포함한 키 생성 (예: "BGM/Title.wav")
					char szSoundKey[256];
					sprintf_s(szSoundKey, "%s%s", szRelativePath, fd.name);

					int iLength = strlen(szSoundKey) + 1;
					TCHAR* pSoundKey = new TCHAR[iLength];
					ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

					MultiByteToWideChar(CP_ACP, 0, szSoundKey, iLength, pSoundKey, iLength);

					m_mapSound.insert({ pSoundKey, pSound });

					///mp3 파일 받아오기
					_finddata_t fd;
					long handle = _findfirst("../Sound/*.*", &fd);

					// WAV, MP3 모두 동일하게 로드
					FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
				}
			}
		}

		iResult = _findnext(handle, &fd);
	}

	_findclose(handle);
}
