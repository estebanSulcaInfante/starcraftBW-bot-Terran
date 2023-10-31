#pragma once

#include "MapTools.h"
#include <vector>
#include <queue>
#include <BWAPI.h>
#include <memory>

class Action
{
protected:
	BWAPI::UnitType type;
	int supplyTrigger;
	int mineralCost;
	int gasCost;
public:
	Action(BWAPI::UnitType type, int supplyTrigger);
	virtual void execute() = 0;
	int getSupplyTrigger();
	int getMineralCost();
	int getGasCost();
};


class BuildAction : public Action
{
	BWAPI::TilePosition buildPosition;
public:
	
	
	BuildAction(BWAPI::UnitType type, int supplyTrigger, BWAPI::TilePosition buildPosition);
	
	
	virtual void execute() override;
};


class TrainAction : public Action
{
public:

	TrainAction(BWAPI::UnitType type, int supplyTrigger);
	
	
	virtual void execute() override;
};


/// <summary>
/// Esta clase maneja la Build Order usando una cola de acciones
/// </summary>
class BuildOrder
{
	std::queue<std::unique_ptr<Action>> actions;

public:
	void addBuildAction(BWAPI::UnitType unitType, BWAPI::TilePosition buildPosition, int supplyTrigger = -1);
	void addTrainAction(BWAPI::UnitType unitType, int supplyTrigger = -1);
	

	void executeNextAction();

	

};

class StarterBot
{
    MapTools m_mapTools;
	

	// helper functions to get you started with bot programming and learn the API
	void sendIdleWorkersToMinerals(); // Dave
    void trainAdditionalWorkers(); // Dave
    void buildAdditionalSupply(); // Dave
    
	void drawDebugInformation();// modify
	void drawPositionsOfAllUnits(); // 
public:

    StarterBot();

    // functions that are triggered by various BWAPI events from main.cpp

	void onStart();
	void onFrame();
	void onEnd(bool isWinner);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
	void onSendText(std::string text);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);

};



