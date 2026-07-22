#pragma once
#include "Define.h"
#include "CScene.h"

class CObj;

class CStage : public CScene
{
public:
	CStage();
	virtual ~CStage();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
private:
	void Render_Pylon_Power(HDC hDC);
	void Render_Shuttle_Power(HDC hDC);
public:
	void LoadTerranResource();
	void LoadZergResource();
	void LoadProtossResource();
	void SpawnTerranBase();
	void SpawnProtossBase();
	void SpawnEnemyBase();
	void SpawnEnemyTerranBase();
	void SpawnEnemyProtossBase();
	void SpawnEnemyZergBase();
	void SpawnZergBase();
	void CreateObject();
	void CreateZergBuildings();
	void CreateTerrranEnemyTest();
	void CreateZergEnemyTest();
	void ClampMouse(HWND hWnd);
	CObj* FindObjectAtPosition(Vec2& worldPos);
private:
	void CreateResource();
private:
	void CheckEndGame();
	void StartRush();
};