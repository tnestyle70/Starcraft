#include "pch.h"
#include "CResourceLoader.h"
#include "CBmpMgr.h"

CResourceLoader* CResourceLoader::m_pInstance = nullptr;

void CResourceLoader::LoadMenuResource()
{
	//싱글, 에디터, Exit 버튼
	//PNG 버튼 이미지 로딩
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/single_button0.png", L"Btn_Single_Normal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/single_button1.png", L"Btn_Single_Hover");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/multiplayer_button.png", L"Btn_Multiplayer_Normal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/editor_button0.png", L"Btn_Editor_Normal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/editor_button1.png", L"Btn_Editor_Hover");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/exit_button0.png", L"Btn_Exit_Normal");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/MainMenu/Button/exit_button1.png", L"Btn_Exit_Hover");
	//SinglePlay 이후 버튼
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Logo/SelectMenu1.png", L"Select_Menu");

	//싱글 애니메이션
	for (int i = 0; i < 35; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/SinglePlay/Animation/single%d.png", i);
		wsprintf(szKey, L"Single_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i < 60; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/SinglePlay/MouseOn/singleon%d.png", i);
		wsprintf(szKey, L"Single_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//멀티 플레이 애니메이션
	for (int i = 0; i < 50; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/MultyPlay/Animation/%d.png", i);
		wsprintf(szKey, L"Multy_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i < 50; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/MultyPlay/MouseOn/%d.png", i);
		wsprintf(szKey, L"Multy_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//에디터 애니메이션
	for (int i = 0; i < 85; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/Editor/Animation/editor%d.png", i);
		wsprintf(szKey, L"Editor_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i < 20; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/Editor/MouseOn/editoron%d.png", i);
		wsprintf(szKey, L"Editor_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Exit 애니메이션
	for (int i = 0; i < 50; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/Exit/Animation/exit%d.png", i);
		wsprintf(szKey, L"Exit_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i < 30; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu/Exit/MouseOn/exiton%d.png", i);
		wsprintf(szKey, L"Exit_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
}

void CResourceLoader::LoadMenuRaceResource()
{
	//프로토스 버튼
	for (int i = 0; i <= 69; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/Protoss/%d.png", i);
		wsprintf(szKey, L"Protoss_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i <= 30; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/ProtossMouseOn/%d.png", i);
		wsprintf(szKey, L"Protoss_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//테란 버튼
	for (int i = 0; i <= 77; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/Terran/%d.png", i);
		wsprintf(szKey, L"Terran_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i <= 30; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/TerranMouseOn/%d.png", i);
		wsprintf(szKey, L"Terran_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//저그 버튼
	for (int i = 0; i <= 69; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/Zerg/%d.png", i);
		wsprintf(szKey, L"Zerg_Normal_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	for (int i = 0; i <= 19; ++i)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/MainMenu2/ZergMouseOn/%d.png", i);
		wsprintf(szKey, L"Zerg_Hover_Anim_%d", i);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
}

void CResourceLoader::LoadProtossResource()
{
	//프로토스 메인 메뉴 
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/MainUI/MainUI.png", L"PROTOSS_MAIN_UI");
	//쉴드 rect
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Bar/ShieldBar/Shield_Rect.png", L"Shield_Rect");
	//Mp Rect
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Bar/MPBar/MpRect.png", L"Mp_Rect");
	//프로브 커맨드 슬롯 버튼
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/LBuild/LBuild_0.png", L"LBuild_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/LBuild/LBuild_1.png", L"LBuild_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/HBuild/HBuild_0.png", L"ABuild_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/HBuild/HBuild_1.png", L"ABuild_1");
	//LBuilding Button
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Nexus/Nexus_0.png", L"Nexus_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Nexus/Nexus_1.png", L"Nexus_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Pylon/Pylon_0.png", L"Pylon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Pylon/Pylon_1.png", L"Pylon_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Gateway/Gateway_0.png", L"Gateway_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Gateway/Gateway_1.png", L"Gateway_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Assimilator/Assimilator_0.png", L"Assimilator_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Assimilator/Assimilator_1.png", L"Assimilator_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Gateway/Gateway_0.png", L"Gateway_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Gateway/Gateway_1.png", L"Gateway_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Forge/Forge_0.png", L"Forge_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Forge/Forge_2.png", L"Forge_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Photon Cannon/Photon Cannon_0.png", L"Photon_Cannon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Photon Cannon/Photon Cannon_2.png", L"Photon_Cannon_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Photon Cannon/Photon Cannon_2.png", L"Photon_Cannon_2");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Cybernetics Core/Cybernetics Core_0.png", L"Cybernetics_Core_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Cybernetics Core/Cybernetics Core_2.png", L"Cybernetics_Core_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Cybernetics Core/Cybernetics Core_2.png", L"Cybernetics_Core_2");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Shield Battery/Shield Battery_0.png", L"Shield_Battery_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Shield Battery/Shield Battery_2.png", L"Shield_Battery_1");
	//RBuilding Button
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Robotics Facility/Robotics Facility_0.png", L"Robotics_Facility_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Robotics Facility/Robotics Facility_2.png", L"Robotics_Facility_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Stargate/Stargate_0.png", L"Stargate_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Stargate/Stargate_2.png", L"Stargate_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Citadel of Adun/Citadel of Adun_0.png", L"CitadelOfAdun_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Citadel of Adun/Citadel of Adun_2.png", L"CitadelOfAdun_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Robotics Support Bay/Robotics Support Bay_0.png", L"RoboticsSupportBay_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Robotics Support Bay/Robotics Support Bay_2.png", L"RoboticsSupportBay_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Fleet Beacon/Fleet Beacon_0.png", L"Fleet_Beacon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Fleet Beacon/Fleet Beacon_2.png", L"Fleet_Beacon_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Tempar Archives/Tempar Archives_0.png", L"Templar_Archives_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Tempar Archives/Tempar Archives_2.png", L"Templar_Archives_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Observatory/Observatory_0.png", L"Observatory_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Observatory/Observatory_2.png", L"Observatory_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter Tribunal/Arbiter Tribunal_0.png", L"Arbiter_Tribunal_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter Tribunal/Arbiter Tribunal_2.png", L"Arbiter_Tribunal_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Cancel/Cancel_0.png", L"Cancle_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Cancel/Cancel_1.png", L"Cancle_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter/Arbiter_0.png", L"Arbiter_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter/Arbiter_1.png", L"Arbiter_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter/Arbiter_2.png", L"Arbiter_2");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Archon/Archon_0.png", L"Archon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Archon/Archon_1.png", L"Archon_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Archon/Archon_2.png", L"Archon_2");


	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Move/move_0.png", L"Move_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Move/move_1.png", L"Move_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Stop/stop_0.png", L"Stop_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Stop/stop_1.png", L"Stop_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Hold/Hold_0.png", L"Hold_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Hold/Hold_1.png", L"Hold_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Patrol/patrol_0.png", L"Patrol_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Patrol/patrol_1.png", L"Patrol_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Att/att_0.png", L"Attack_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Att/att_1.png", L"Attack_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Rally/Rally_0.png", L"Rally_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Rally/Rally_1.png", L"Rally_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Storm/Storm_0.png", L"Storm_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Storm/Storm_2.png", L"Storm_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Archon/Archon_0.png", L"Wrap_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Archon/Archon_2.png", L"Wrap_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Hal/hal0.png", L"Hallucination_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Hal/hal1.png", L"Hallucination_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Interceptor/Interceptor_0.png", L"Interceptor_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Interceptor/Interceptor_1.png", L"Interceptor_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Recall/Recall_0.png", L"Recall_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Recall/Recall_2.png", L"Recall_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Ice/Ice_0.png", L"Ice_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Ice/Ice_1.png", L"Ice_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Ice/Ice_0.png", L"Ice_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Ice/Ice_1.png", L"Ice_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Boarding/Boarding_0.png", L"Loading_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Boarding/Boarding_1.png", L"Loading_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Landing/Landing_0.png", L"Landing_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Landing/Landing_1.png", L"Landing_1");

	//질럿 돌진
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/ZealotSpeedUP/ZealotSpeedUP_0.png", L"Rush_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/ZealotSpeedUP/ZealotSpeedUP_1.png", L"Rush_1");
	//드라군
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Dragoon/Dragoon_0.png", L"Dragoon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Dragoon/Dragoon_2.png", L"Dragoon_1");
	//드라군 점멸(RangeUP)
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/RangeUP/RangeUP_0.png", L"Blink_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/RangeUP/RangeUP_1.png", L"Blink_1");
	//유닛 버튼
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Probe/Probe_0.png", L"Probe_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Probe/Probe_1.png", L"Probe_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Zealot/Zealot_0.png", L"Zealot_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Zealot/Zealot_1.png", L"Zealot_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Dragoon/Dragoon_0.png", L"Dragoon_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Dragoon/Dragoon_2.png", L"Dragoon_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/HighTemplar/HighTemplar_0.png", L"HighTemplar_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/HighTemplar/HighTemplar_2.png", L"HighTemplar_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/DarkTemplar/DarkTemplar_0.png", L"DarkTemplar_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/DarkTemplar/DarkTemplar_2.png", L"DarkTemplar_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Observer/Observer_0.png", L"Observer_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Observer/Observer_2.png", L"Observer_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Shuttle/Shuttle_0.png", L"Shuttle_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Shuttle/Shuttle_2.png", L"Shuttle_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Reaver/Reaver_0.png", L"Reaver_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Reaver/Reaver_2.png", L"Reaver_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Corsair/Corsair_0.png", L"Corsair_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Corsair/Corsair_2.png", L"Corsair_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Scout/Scout_0.png", L"Scout_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Scout/Scout_2.png", L"Scout_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Carrier/Carrier_0.png", L"Carrier_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Carrier/Carrier_2.png", L"Carrier_1");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter/Arbiter_0.png", L"Arbiter_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Arbiter/Arbiter_2.png", L"Arbiter_1");

	//다크 아칸
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/MindControl/MindControl_0.png", L"MindControl_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/MindControl/MindControl_2.png", L"MindControl_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Maelstrom/Maelstrom0.png", L"Maelstrom_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Maelstrom/Maelstrom0.png", L"Maelstrom_1");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Feedback/Feedback0.png", L"Feedback_0");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/Button/Feedback/Feedback0.png", L"Feedback_1");

	//프로토스 건물
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Nexus/Img/Nexus_0.png", L"Nexus");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Nexus/BigWire_06.png", L"Nexus_BigWire");

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Protoss/Building/Pylon/Img/Pylon_0_bmp.bmp", L"Pylon");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Pylon/BigWire_06.png", L"Pylon_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Pylon/Range/Pylon_0_range.png", L"Pylon_Range");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Assimilator/Img/Assimilator_0.png", L"Assimilator");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Gateway/Img/Gateway_0.png", L"Gateway");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Gateway/BigWire_06.png", L"Gateway_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Forge/Img/Forge_0.png", L"Forge");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Forge/BigWire_06.png", L"Forge_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Photon Cannon/Img/Photon Cannon_3.png", L"Photon_Cannon");


	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Cybernetics Core/Img/Cybernetics Core_0_bmp.bmp", L"Cybernetics_Core");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Core/BigWire_06.png", L"Core_BigWire");

	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Shield Battery/Img/Shield Battery_0.png", L"Shield_Battery");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Battery/BigWire_06.png", L"ShieldBattery_BigWire");
	//ABuilding
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Robotics Facility/Img/Robotics Facility_0.png", L"Robotics_Facility");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Stargate/Img/Stargate_0.png", L"Stargate");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Citadel of Adun/Img/Citadel of Adun_0.png", L"CitadelOfAdun");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Robotics Support Bay/Img/Robotics Support Bay_0.png", L"Robotics_Support_Bay");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Fleet Beacon/Img/Fleet Beacon_0.png", L"Fleet_Beacon");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Tempar Archives/Img/Tempar Archives_0.png", L"Templar_Archives");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Observatory/Img/Observatory_0.png", L"Observatory");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/Building/Arbiter Tribunal/Img/Arbiter Tribunal_0.png", L"Arbiter_Tribunal");

	//프로토스 유닛 건물 초상화
	for (int r = 5; r < 75; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/UI/Portraits/Aldaris/Portraits_%d.png", r);
		wsprintf(szKey, L"Protoss_Portraits_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//빌드 템플릿
	for (int r = 0; r < 14; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Warp/warp%d.png", r);
		wsprintf(szKey, L"Protoss_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//넥서스 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Nexus/BuildTime/nexus%d.png", r);
		wsprintf(szKey, L"Nexus_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}

	//파일런 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Pylon/BuildTime/pylon%d.png", r);
		wsprintf(szKey, L"Pylon_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Assimilator 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Assimilator/BuildTime/assim%d.png", r);
		wsprintf(szKey, L"Assimilator_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Gateway 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Gateway/BuildTime/gateway%d.png", r);
		wsprintf(szKey, L"Gateway_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Forge 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Forge/BuildTime/forge%d.png", r);
		wsprintf(szKey, L"Forge_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Cybernetics_Core 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Cybernetics Core/BuildTime/core%d.png", r);
		wsprintf(szKey, L"Cybernetics_Core_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Shield Battery 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Shield Battery/BuildTime/battery%d.png", r);
		wsprintf(szKey, L"ShieldBattery_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Shield Battery 치유 이펙트
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Hallucination/hallucination_%d.png", r);
		wsprintf(szKey, L"ShieldBattery_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Robotics 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Robotics Facility/BuildTime/robot%d.png", r);
		wsprintf(szKey, L"Robotics_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Stargate 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Stargate/BuildTime/stgate%d.png", r);
		wsprintf(szKey, L"Stargate_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//CitadelOfAdun 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Citadel of Adun/BuildTime/adun%d.png", r);
		wsprintf(szKey, L"CitadelOfAdun_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Robotics Support Bay 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Robotics Support Bay/BuildTime/robotsp%d.png", r);
		wsprintf(szKey, L"RoboticsSupportBay_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Fleet Beacon 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Fleet Beacon/BuildTime/beacon%d.png", r);
		wsprintf(szKey, L"FleetBeacon_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Templar Archives 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Tempar Archives/BuildTime/templar%d.png", r);
		wsprintf(szKey, L"TemplarArchives_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Observatory 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Observatory/BuildTime/observ%d.png", r);
		wsprintf(szKey, L"Observatory_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//Arbiter Tribunal 빌드 템플릿
	for (int r = 0; r < 21; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Building/Arbiter Tribunal/BuildTime/arbiter%d.png", r);
		wsprintf(szKey, L"ArbiterTribunal_Build_Template_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//프로브
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Probe/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Probe_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//프로브 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Probe/BigWire_06.png", L"Probe_BigWire");
	//프로브 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Probe/grpwire06.png", L"Probe_SmallWire");
	//셔틀
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Shuttle/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Shuttle_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//셔틀 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Shuttle/BigWire_06.png", L"Shuttle_BigWire");
	//셔틀 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Shuttle/grpwire86.png", L"Shuttle_SmallWire");
	//질럿
	for (int frame = 0; frame < 7; ++frame)
	{
		for (int dir = 0; dir < 17; ++dir)
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Zealot/Move/zealot%d.png", index);
			wsprintf(szKey, L"Zealot_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//질럿 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Zealot/BigWire_06.png", L"Zealot_BigWire");
	//질럿 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Zealot/grpwire16.png", L"Zealot_SmallWire");
	//질럿 attack
	for (int frame = 0; frame < 7; ++frame)
	{
		for (int dir = 0; dir < 17; ++dir)
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Zealot/Att/zealot%d.png", index);
			wsprintf(szKey, L"Zealot_Attack%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//아칸
	for (int frame = 0; frame < 4; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Archon/Normal/normal_%d.png", index);
			wsprintf(szKey, L"Archon_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//아칸 몸체
	for (int r = 0; r < 10; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Archon/Body/body_%d.png", r);
		wsprintf(szKey, L"Archon_Body_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//아칸 Attack
	for (int frame = 0; frame < 10; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Archon/Att/att_%d.png", index);
			wsprintf(szKey, L"Archon_Attack%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//아칸 Create
	for (int r = 0; r < 10; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Archon/Create/create_%d.png", r);
		wsprintf(szKey, L"Archon_Create_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//아칸 Shockball
	for (int r = 0; r < 6; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Shockball/Shockball_%d.png", r);
		wsprintf(szKey, L"Archon_Shockball_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Dark Archon/Normal/normal_%d.png", r);
		wsprintf(szKey, L"DarkArchon_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/DarkArchon/BigWire_06.png", L"DarkArchon_BigWire");
	//다크 아칸 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/DarkArchon/grpwire66.png", L"DarkArchon_SmallWire");
	//다크 아칸 몸체
	for (int r = 0; r < 10; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Dark Archon/Body/body_%d.png", r);
		wsprintf(szKey, L"DarkArchon_Body_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 아우라
	for (int r = 0; r < 15; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Dark Archon/Att/att_%d.png", r);
		wsprintf(szKey, L"DarkArchon_Aura_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 Attack
	for (int frame = 0; frame < 10; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Dark Archon/Att/Att/att_%d.png", index);
			wsprintf(szKey, L"DarkArchon_Attack%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//다크 아칸 공격 이펙트
	for (int frame = 0; frame < 3; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Effect/Shockwave_red/Shockwave/shock_%d.png", index);
			//wsprintf(szPath, L"../Image/Protoss/Effect/Shockwave/shock_%d.png", index);
			wsprintf(szKey, L"DarkArchon_Shockwave%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//다크 아칸 Shock_Ball
	for (int r = 0; r < 6; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Shockball_red/Shockball/Shockball_%d.png", r);
		wsprintf(szKey, L"DarkArchon_Shockball_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 Create
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Dark Archon/Create/create_%d.png", r);
		wsprintf(szKey, L"DarkArchon_Create_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 마인드 컨트롤 이펙트
	for (int r = 0; r < 20; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/MindControl/mindcontrol_%d.png", r);
		wsprintf(szKey, L"MindControl_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크 아칸 DIE
	for (int r = 0; r < 15; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/DarkArchonDie/die_%d.png", r);
		wsprintf(szKey, L"DArchon_Die_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크템플러 move
	for (int frame = 0; frame < 8; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/DarkTemplar/Move/move_%d.png", index);
			wsprintf(szKey, L"DarkTemplar_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//다크 템플러 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/DarkTemplar/BigWire_06.png", L"DarkTemplar_BigWire");
	//다크 템플러 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/DarkTemplar/grpwire56.png", L"DarkTemplar_SmallWire");
	//다크 템플러 DIE
	for (int r = 0; r < 7; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/DarkTemplarDie/die_%d.png", r);
		wsprintf(szKey, L"DarkTemplar_Die_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//다크템플러 attack
	for (int frame = 0; frame < 10; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/DarkTemplar/Att/att_%d.png", index);
			wsprintf(szKey, L"DarkTemplar_Attack%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//드라군
	for (int frame = 0; frame < 8; ++frame)  // 동작 프레임 0~3
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Dragoon/Move/move_%d.png", index);
			wsprintf(szKey, L"Dragoon_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//다크 템플러 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Dragoon/BigWire_06.png", L"Dragoon_BigWire");
	//다크 템플러 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Dragoon/grpwire26.png", L"Dragoon_SmallWire");
	//드라군 att
	for (int r = 0; r < 8; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Dragoon/Att/att_%d.png", r);
		wsprintf(szKey, L"Dragoon_Attack_Anim%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//드라군 ball
	for (int r = 0; r < 5; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Ball/Ball_%d.png", r);
		wsprintf(szKey, L"Dragoon_Ball_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//드라군 ball bmp
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Protoss/Effect/Ball_bmp/Ball/Ball_0.bmp", L"Dragoon_Ball");

	//드라군 ball ex
	for (int r = 0; r < 14; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/BallEx/ballex_%d.png", r);
		wsprintf(szKey, L"Dragoon_BallEx_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//아비터
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Arbiter/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Arbiter_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//아비터 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Arbiter/BigWire_06.png", L"Arbiter_BigWire");
	//아비터 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Arbiter/grpwire146.png", L"Arbiter_SmallWire");
	//아비터 Recall
	for (int r = 0; r < 11; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Recall/recall%d.png", r);
		wsprintf(szKey, L"Arbiter_Recall_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//캐리어
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Carrier/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Carrier_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//캐리어 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Carrier/BigWire_06.png", L"Carrier_BigWire");
	//캐리어 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Carrier/grpwire116.png", L"Carrier_SmallWire");
	//인터셉터
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Interceptor/Normal/normal_%d.png", r);
		wsprintf(szKey, L"Interceptor_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//인터셉터 attack
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Interceptor/Att/att_%d.png", r);
		wsprintf(szKey, L"Interceptor_Attack_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//인터셉터 bullet
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Intercep_Bullet/bullet_%d.png", r);
		wsprintf(szKey, L"Interceptor_Bullet_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//커세어
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Corsair/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Corsair_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//커세어 Att
	for (int frame = 0; frame < 4; ++frame)  // 동작 프레임 0~4
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Corsair/Att/att_%d.png", index);
			wsprintf(szKey, L"Corsair_Attack%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//커세어 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Corsair/BigWire_06.png", L"Corsair_BigWire");
	//커세어 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Corsair/grpwire136.png", L"Corsair_SmallWire");
	//하이템플러
	for (int frame = 0; frame < 4; ++frame)  // 동작 프레임 0~4
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/HighTemplar/Move/move_%d.png", index);
			wsprintf(szKey, L"HighTemplar_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//하이 템플러 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/HighTemplar/BigWire_06.png", L"HighTemplar_BigWire");
	//하이 템플러 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/HighTemplar/grpwire36.png", L"HighTemplar_SmallWire");
	//하이템플러 그림자
	for (int frame = 0; frame < 3; ++frame)  // 동작 프레임 0~4
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/HighTemplar/Shad/shad_%d.png", index);
			wsprintf(szKey, L"HighTemplar_Shad%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//하이템플러 스톰
	for (int r = 0; r < 14; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Storm/Storm_%d.png", r);
		wsprintf(szKey, L"Storm_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//하이템플러 DIE
	for (int r = 0; r < 7; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/HighTemplarDie/die_%d.png", r);
		wsprintf(szKey, L"HighTemplar_Die_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//리버
	for (int frame = 0; frame < 9; ++frame)  // 동작 프레임 0~4
	{
		for (int dir = 0; dir < 17; ++dir)  // 각 동작의 17장
		{
			int index = frame * 17 + dir;
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Protoss/Unit/Reaver/Move/move_%d.png", index);
			wsprintf(szKey, L"Reavor_Move%d_Anim%d", dir, frame);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//리버 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Reaver/BigWire_06.png", L"Reaver_BigWire");
	//리버 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Reaver/grpwire76.png", L"Reaver_SmallWire");
	//스카웃
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Scout/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Scout_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//스카웃 Attack
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Scout/Att/att_%d.png", r);
		wsprintf(szKey, L"Scout_Attack_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//스카웃 ground effect
	for (int r = 0; r < 8; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Effect/Scout_Ground/ground_%d.png", r);
		wsprintf(szKey, L"Scout_GroundEffect_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//스카웃 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Scout/BigWire_06.png", L"Scout_BigWire");
	//스카웃 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Scout/grpwire106.png", L"Scout_SmallWire");
	//옵저버
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Protoss/Unit/Observer/Normal/Normal_%d.png", r);
		wsprintf(szKey, L"Observer_Move_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//옵저버 빅 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/BigWire/Observer/BigWire_06.png", L"Observer_BigWire");
	//옵저버 미니 와이어 프레임
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Protoss/UI/WireFrame/SmallWire/Observer/grpwire96.png", L"Observer_SmallWire");
}

void CResourceLoader::LoadZergResource()
{
	//오버로드 
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 4; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Zerg/Overload/Move/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Overload_Move%d_Anim%d", r, c);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Overload/Wire/0.png", L"OVERLOAD_WIRE");
	//뮤탈리스크 move
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 5; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Zerg/Mutalisk/Move/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Mutalisk_Move%d_Anim%d", r, c);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//뮤탈리스크 Hit effect
	for (int r = 0; r < 10; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Mutalisk/HitEffect/%d.png", r);
		wsprintf(szKey, L"Mutalisk_HitEffect_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//뮤탈리스크 portrait
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Mutalisk/Portrait/%d.png", r);
		wsprintf(szKey, L"Mutalisk_Portrait_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//뮤탈리스크 Bullet
	for (int r = 0; r < 10; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Mutalisk/AttackBullet/%d.png", r);
		wsprintf(szKey, L"Mutalisk_Bullet_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Mutalisk/Wire/0.png", L"MUTALISK_WIRE");
	// 저글링 Attack
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 4; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Zerg/Zergling/Attack/Attack%d/%d.png", r, c);
			wsprintf(szKey, L"Zergling_Attack%d_Anim%d", r, c);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//저글링 portrait
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Zergling/Portrait/%d.png", r);
		wsprintf(szKey, L"Zergling_Portrait_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	// 저글링 Move
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 8; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];
			wsprintf(szPath, L"../Image/Zerg/Zergling/Move/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Zergling_Move%d_Anim%d", r, c);
			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Zergling/Wire/0.png", L"ZERGLING_WIRE");
	//히드라 Attack
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 5; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/Zerg/Hydralisk/Attack/Attack%d/%d.png", r, c);
			wsprintf(szKey, L"Hydralisk_Attack%d_Anim%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//히드라 portrait
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Hydralisk/Portrait/%d.png", r);
		wsprintf(szKey, L"Hydralisk_Portrait_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//히드라 Move
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 7; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/Zerg/Hydralisk/Move/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Hydralisk_Move%d_Anim%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//히드라 Attack Bullet
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 7; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/Zerg/Hydralisk/AttackBullet/Attack%d/%d.png", r, c);
			wsprintf(szKey, L"Hydralisk_Attack%d_Bullet%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//히드라 hit effect
	for (int r = 0; r < 8; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];

		wsprintf(szPath, L"../Image/Zerg/Hydralisk/HitEffect/%d.png", r);
		wsprintf(szKey, L"Hydralisk_HitEffect_%d", r);

		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Hydralisk/Wire/0.png", L"HYDRALISK_WIRE");
	//울트라 move
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 9; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/Zerg/Ultralisk/Move/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Ultralisk_Move%d_Anim%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//울트라 portrait
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Ultralisk/Portrait/%d.png", r);
		wsprintf(szKey, L"Ultralisk_Portrait_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//울트라 attack
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 6; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/Zerg/Ultralisk/Attack/Attack%d/%d.png", r, c);
			wsprintf(szKey, L"Ultralisk_Attack%d_Anim%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Ultralisk/Wire/0.png", L"ULTRALISK_WIRE");
	//하이브
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Hive/0.png", L"HIVE");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Hive/Wire/0.png", L"HIVE_WIRE");
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/Hive/Portrait/%d.png", r);
		wsprintf(szKey, L"Hive_Portrait_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//드론 move
	for (int r = 0; r < 16; ++r)
	{
		for (int c = 0; c < 3; ++c)
		{
			TCHAR szPath[256];
			TCHAR szKey[256];

			wsprintf(szPath, L"../Image/ZergRes/Texture/Multi/Unit/Drone/Move%d/%d.png", r, c);
			wsprintf(szKey, L"Drone_Move%d_Anim%d", r, c);

			CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
		}
	}
	//히드라리스크덴
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/HydraliskDen/0.png", L"HYDRALISK_DEN");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/HydraliskDen/Wire/0.png", L"HYDRALISK_DEN_WIRE");
	//스파이어
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Spire/0.png", L"SPIRE");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Spire/Wire/0.png", L"SPIRE_WIRE");
	//스포닝풀
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/SpawningPool/0.png", L"SPAWNING_POOL");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/SpawningPool/Wire/0.png", L"SPAWNING_POOL_WIRE");
	//울트라리스크덴
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Build_Ultralisk/0.png", L"ULTRALISK_DEN");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Zerg/Build_Ultralisk/Wire/0.png", L"ULTRALISK_DEN_WIRE");
	//저그 빌딩 파괴 애니메이션
	for (int r = 0; r < 12; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Zerg/BuildDeath/%d.png", r);
		wsprintf(szKey, L"BUILDING_DESTROY_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
}

void CResourceLoader::LoadTerranResource()
{
	//스테이지 이미지
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MyFolder/Map/FightSpirit.bmp", L"Stage");
	//마린 스프라이트 시트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Marine/Marine.bmp", L"Marine");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Marine/MarineGunSpark.bmp", L"MarineHit");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Marine/MarineDeathEffect.bmp", L"MARINE_DEATH_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Marine/Marine.bmp", L"MARINE_PORTRAIT");
	//배틀크루저 스프라이트 시트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/BattleCruiser/BattleCruiser.bmp", L"BattleCruiser");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/BattleCrusor/BattleAttack.bmp", L"BCBullet");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/BattleCrusor/Yamato.bmp", L"BCYamato");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Battlecrusor/BattleCrusor.bmp", L"BATTLECRUISER_PORTRAIT");
	//탱크 스프라이트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Tank/TankBody.bmp", L"Tank_Body");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Tank/TankHead.bmp", L"Tank_Head");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/TankAtt/TankHit.bmp", L"TankHit");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/TankAtt/SiegeTankHit.bmp", L"SiegeTankHit");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Tank/Tank.bmp", L"TANK_PORTRAIT");
	//SCV
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/SCV/SCV.bmp", L"SCV");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/SCV/SCVEffect.bmp", L"SCV_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/SCV/SCV.bmp", L"SCV_PORTRAIT");
	//Medic
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Medic/Medic.bmp", L"Medic");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Medic/MedicDeathEffect.bmp", L"MEDIC_DEATH_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Medic/Medic.bmp", L"MEDIC_PORTRAIT");
	//Vulture
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Vulture/Vulture.bmp", L"Vulture");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Bullet/VultureAtt/grenade0.png", L"VBullet");
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szKey[128];
		TCHAR szPath[256];

		wsprintf(szKey, L"Vulture_Portrait_%d", r);
		wsprintf(szPath, L"../Image/UI/Portrait/Vulture/%d.bmp", r);

		CBmpMgr::Get_Instance()->Insert_Bmp_Anim(szPath, szKey);
	}
	//Goliath
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Goliath/GoliathBody.bmp", L"Goliath_Body");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Goliath/GoliathHead.bmp", L"Goliath_Head");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Bullet/MissileAtt/missile0.png", L"GBullet");
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Bullet/BomberAtt/gemini%d.png", r);
		wsprintf(szKey, L"Missile_%d", r);
		CBmpMgr::Get_Instance()->Insert_Bmp(szPath, szKey);
	}
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szKey[128];
		TCHAR szPath[256];

		wsprintf(szKey, L"Goliath_Portrait_%d", r);
		wsprintf(szPath, L"../Image/UI/Portrait/Goliath/%d.bmp", r);

		CBmpMgr::Get_Instance()->Insert_Bmp_Anim(szPath, szKey);
	}
	//Firebat
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/FireBat/Firebat.bmp", L"FIREBAT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/FireBat/FireBatHit.bmp", L"FIREBAT_HIT_EFFECT");
	for (int r = 0; r < 45; ++r)
	{
		TCHAR szKey[128];
		TCHAR szPath[256];

		wsprintf(szKey, L"FireBat_Portrait_%d", r);
		wsprintf(szPath, L"../Image/UI/Portrait/FireBat/%d.bmp", r);

		CBmpMgr::Get_Instance()->Insert_Bmp_Anim(szPath, szKey);
	}
	//Ghost
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/Ghost/Ghost.bmp", L"GHOST");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Ghost/GhostShot.bmp", L"GHOST_SHOT_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Ghost/GhostDeathEffect.bmp", L"GHOST_DEATH_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/NukeMissile/NuclearMissleDown.bmp", L"GHOST_NUCLEAR_BULLET");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Effect/Nuclear/NuclearEffect.bmp", L"GHOST_NUCLEAR_EFFECT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Ghost/Ghost.bmp", L"GHOST_PORTRAIT");
	//커맨드 센터
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/CommandCenter/CommandCenter.bmp", L"CommandCenter");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/CommandCenter.bmp", L"COMMANDCENTER_CONSTRUCT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/Portrait/Advisor/Advisor.bmp", L"ADVISOR_PORTRAIT");
	//블루 커맨드 센터 
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/trash/Enemy/Blue_CommandCenter.bmp", L"Blue_CommandCenter");
	//블루 마린
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Unit/E_Unit/MarineBlue.bmp", L"BLUE_MARINE");
	//배럭
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Barrack/Barrack.bmp", L"Barracks");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Barrack.bmp", L"BARRACKS_ANIM");
	//스타포트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Starport/Starport.bmp", L"Starport");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Starport/StarportAddOn.bmp", L"STARPORT_ADDON");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Starport.bmp", L"STARPORT_ANIM");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/StarportAddOn.bmp", L"STARPORT_ADDON_ANIM");
	//보급고
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/SupplyDepot/SupplyDepot.bmp", L"SupplyDepot");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/SupplyDepot.bmp", L"SUPPLYDEPOT_ANIM");
	//팩토리
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Factory/Factory.bmp", L"Factory");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Factory/FactoryAddOn.bmp", L"FACTORY_ADDON");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Factory.bmp", L"FACTORY_ANIM");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/FactoryAddOn.bmp", L"FACTORY_ADDON_ANIM");
	//정제소
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Refinery/Refinery4.bmp", L"Refinery");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Refinery.bmp", L"Refinery_Anim");
	//벙커
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Bunker/Bunker.bmp", L"BUNKER");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Bunker.bmp", L"BUNKER_ANIM");
	//터렛
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Turret/Turret.bmp", L"TURRET");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Turret.bmp", L"TURRET_ANIM");
	for (int r = 0; r < 17; ++r)
	{
		TCHAR szPath[256];
		TCHAR szKey[256];
		wsprintf(szPath, L"../Image/Bullet/TurretAtt/hks%d.png", r);
		wsprintf(szKey, L"Turret_Missile_%d", r);
		CBmpMgr::Get_Instance()->Insert_Png_Anim(szPath, szKey);
	}
	//엔지니어링베이
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/EngineeringBay/EngineeringBay.bmp", L"ENGINEERING_BAY");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/EngineeringBay.bmp", L"ENGINEERING_BAY_ANIM");
	//아머리
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Armory/Armory.bmp", L"ARMORY");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Armory.bmp", L"ARMORY_ANIM");
	//아카데미
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Academy/Academy.bmp", L"ACADEMY");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/Academy.bmp", L"ACADEMY_ANIM");
	//Scienece Facility
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Facility/ScienceFacility.bmp", L"SCIENCE_FACILITY");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Facility/SciencePhysics2.bmp", L"SCIENCE_PHYSICS");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/Facility/ScienceSecret.bmp", L"SCIENCE_SECRET");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/ScienceFacility.bmp", L"SCIENCE_FACILITY_ANIM");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/SciencePhysics.bmp", L"SCIENCE_PHYSICS_ANIM");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildAnim/ScienceSecret.bmp", L"SCIENCE_SECRET_ANIM");
	//빌딩 템플릿 
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Build/BuildTemplate.bmp", L"BUILD_TEMPLATE");
	//미네랄, 가스 
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Resource/Mineral0.bmp", L"Mineral");
	//CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Resource/mineral.bmp", L"Mineral");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Resource/mineral.png", L"MINERAL");
	CBmpMgr::Get_Instance()->Insert_Png(L"../Image/Resource/miniGas.png", L"GAS");
	//Wintext
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Win/WinText.bmp", L"WIN_TEXT");
}
