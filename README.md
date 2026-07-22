# StarCraft WinAPI RTS

WinAPI와 GDI만으로 RTS의 핵심 구조를 직접 구현한 개인 프로젝트입니다. 프레임워크가 제공하는 기능에 기대지 않고 입력, 명령, 유닛 상태, 길찾기, 시야, 렌더링과 오브젝트 수명을 한 흐름으로 연결하는 것을 목표로 했습니다.

[범용 포트폴리오](https://shy-scorpio-3d0.notion.site/39cb8c3c75e280b597c0fd2f7aaacc43)

![StarCraft swarm battle](https://raw.githubusercontent.com/tnestyle70/Winters_Engine/main/docs/media/starcraft_swarm_battle.gif)

## 핵심 구조

~~~text
Win32 Input
→ CCommandMgr
→ Unit Order Queue
→ Unit FSM / Navigation / Combat
→ GDI Render
~~~

- `CCommandMgr`: 이동, 공격, Attack-Move 명령 생성과 선택 유닛 배포
- `CUnit`: 명령 큐 소비, 이동·추적·공격 상태 전환
- `CNavMgr`: 8방향 Octile 휴리스틱 기반 A* 경로 탐색
- `CFogMgr`: `UNKNOWN / EXPLORED / VISIBLE` 3상태 전장의 안개와 캐시 DC
- `CObjMgr`: 갱신 중 즉시 삭제를 피하는 pending-delete 수명 관리
- `CTileMgr`·`CEdit`: 타일 편집과 맵 저장·불러오기

## 구현한 범위

- Terran, Protoss, Zerg의 유닛·건물·생산·전투 흐름
- Move, Attack, Attack-Move를 하나의 명령 큐로 연결
- GDI 마스킹·알파 블렌딩 기반 2D 렌더링과 FMOD 사운드
- 선택, 드래그, 충돌, 미니맵, 전장의 안개와 맵 편집 도구
- `CStage → CCommandMgr → CUnit`으로 이어지는 Attack-Move 코드 경로

## 코드 지도

- Attack-Move 입력과 명령 생성: `CCommandMgr.cpp`
- 명령 큐와 상태 전환: `CUnit.cpp`
- A* 탐색: `CNavMgr.cpp`
- 전장의 안개: `CFogMgr.cpp`
- 오브젝트 삭제 지연: `CObjMgr.cpp`
- 메인 게임 흐름: `CStage.cpp`

## 빌드

- Windows, Visual Studio 2022
- `Starcraft.slnx` 또는 `Starcraft.vcxproj`
- 현재 검증 구성: Debug Win32
- FMOD Ex SDK의 헤더·라이브러리·런타임은 저장소에 포함하지 않으며 별도로 연결해야 합니다.
- x64 빌드는 x64용 FMOD 라이브러리가 필요합니다.
- 실행에는 저장소에 포함되지 않은 학습용 게임 리소스가 별도로 필요합니다.

## 현재 한계

- 학습 목적으로 만든 WinAPI/GDI 단일 클라이언트이며 네트워크 대전은 포함하지 않습니다.
- 현대적인 GPU 렌더러나 데이터 지향 ECS 대신, 직접 만든 상속 기반 게임 오브젝트 구조를 사용합니다.
- 코드 구조와 문제 해결 과정을 공개하며, 원작의 상표·저작권과 무관한 비상업 포트폴리오입니다.

## 에셋 고지

서드파티 게임 이미지·사운드 리소스는 저장소에 포함하지 않습니다. 공개 GIF는 구현 결과 설명용입니다.
