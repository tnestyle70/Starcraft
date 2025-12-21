#include "pch.h"
#include "CCommandCenter.h"
#include "CTimeMgr.h"

CCommandCenter::CCommandCenter()
{
}

CCommandCenter::~CCommandCenter()
{
}

void CCommandCenter::Initialize()
{
	CBuilding::Initialize();
}

int CCommandCenter::Update()
{
	int ret = CBuilding::Update();
	if (m_eState == eBuildingState::COMPLETE)
	{
		UpdateProduction();
	}

	return ret;
}

void CCommandCenter::SetBuildingData()
{
	m_eType = eBuildingType::COMMAND_CENTER;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 100;
	m_fConstructDuration = 10.f;
	//타일 단위 크기
	m_iHeight = 4;
	m_iWidth = 4;
}

void CCommandCenter::ConstructComplete()
{
}

void CCommandCenter::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	outSlot.clear();
	outSlot.resize(9);
	//미리 값 채우기
	for (int i = 0; i < 9; ++i)
	{
		outSlot[i].slotIndex = i;
		outSlot[i].commandID = eCommandID::NONE;
		outSlot[i].iconKey = TEXT("");
		outSlot[i].hotkey = 0;
		outSlot[i].clickable = false;
		outSlot[i].visible = false;
	}
	//0번 슬롯 SCV 생산
	outSlot[0].commandID = eCommandID::TRAIN_SCV;
	outSlot[0].iconKey = TEXT("ICON_SCV");
	outSlot[0].hotkey = 'S';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//8번 : Cancle(Queue 취소)
	outSlot[7].commandID = eCommandID::CANCLE;
	outSlot[7].iconKey = TEXT("ICON_CANCLE");
	outSlot[7].hotkey = VK_ESCAPE;
	outSlot[7].clickable = true;
	outSlot[7].visible = true;
}

bool CCommandCenter::ExecuteCommand(eCommandID command, CommandContext& context)
{
	ResourceCost cost{};

	switch (command)
	{
	case eCommandID::TRAIN_SCV:
		cost.mineral = 50;
		cost.gas = 0;
		cost.supply = 1;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::TRAIN_SCV, 12.f });
		return true;
		break;
	case eCommandID::CANCLE:
		//생산 중인 큐 취소
		if (m_queue.empty())
		{
			m_queue.pop_back();
			//환불 정책
			return true;
		}
		return false;
		break;
	default:
		break;
	}
	return false;
}

void CCommandCenter::UpdateProduction()
{
	//건설 완료시 처리(사운드, 이펙트, 기능 오픈 포함 )
	if (m_queue.empty())
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_queue.front().remainTime -= dt;
	
	if (m_queue.front().remainTime <= 0.f)
	{
		eCommandID done = m_queue.front().command;
		m_queue.pop_front();
		ConstructComplete(done);
	}
}


void CCommandCenter::ConstructComplete(eCommandID command)
{
	if (command == eCommandID::TRAIN_SCV)
	{
		//실제 유닛 스폰
		//CObjMgr::Create(SCV)
	}
}

void CCommandCenter::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}
