#pragma once

#include "MapTools.h"
#include <vector>
#include <queue>
#include <BWAPI.h>
#include <memory>


class WorkerManager {
private:
    std::vector<BWAPI::Unit> workers; // Lista de todos los trabajadores
    std::map<BWAPI::UnitType, std::vector<BWAPI::Unit>> workersByTask; // Trabajadores clasificados por tarea
    std::map<BWAPI::Unit, BWAPI::TilePosition> workerBuildTargets; // Destinos de construcci�n para los trabajadores

public:
    WorkerManager();

    // Ejecucion en el juego
	void onStart();
	void onFrame();
	void onUnitCreate();	

	// Asignaci�n y gesti�n de recursos
    void assignWorkerToMinerals(BWAPI::Unit worker);
    void assignWorkerToGas(BWAPI::Unit worker);
    void balanceWorkerAllocation(); // Rebalancea los trabajadores entre minerales y gas

    // B�squeda de trabajadores
    BWAPI::Unit getClosestWorkerTo(BWAPI::Position pos);
    BWAPI::Unit getLeastBusyWorker();

    // Gesti�n de trabajadores en construcci�n
    void assignWorkerToBuild(BWAPI::Unit worker, BWAPI::UnitType buildingType, BWAPI::TilePosition buildPosition);
    void onBuildingCompletion(BWAPI::Unit building); // Llamado cuando un edificio se completa

    // Defensa y protecci�n
    void protectWorkers();
    void evacuateWorkers(BWAPI::Position dangerZone);

    // Optimizaci�n y eficiencia
    void optimizeWorkerEfficiency();

    // Reparaci�n y mantenimiento
    void repairDamagedUnit(BWAPI::Unit unit);

    // Exploraci�n y escaneo del mapa
    void sendWorkerToScout(BWAPI::Position targetPosition);

    // Interfaz y reportes
    std::vector<BWAPI::Unit> getAvailableWorkers();
    void generateWorkerReport();

    // Funciones adicionales seg�n sea necesario
    // ...
};

class Action
{
protected:
	BWAPI::UnitType type;
	int supplyTrigger;
	int mineralCost;
	int gasCost;
public:
	Action(BWAPI::UnitType type, int supplyTrigger);
	virtual bool canExecute()=0;
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
	
	virtual bool canExecute() override;
	virtual void execute() override;
};


class TrainAction : public Action
{
public:

	TrainAction(BWAPI::UnitType type, int supplyTrigger);
	
	virtual bool canExecute() override;
	virtual void execute() override;
};


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
	WorkerManager  workerManager;

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



