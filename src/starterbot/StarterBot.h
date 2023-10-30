#pragma once

#include "MapTools.h"
#include <vector>
#include <queue>
#include <BWAPI.h>

enum ActionType {

};

class Action
{
	int supplyTrigger;
	int mineralCost;
	int gasCost;
};

/// <summary>
/// Esta clase maneja la Build Order usando una cola de acciones
/// </summary>
class BuildOrder
{
	std::queue<Action> actions;
public:
	void addAction(Action action);
	

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



