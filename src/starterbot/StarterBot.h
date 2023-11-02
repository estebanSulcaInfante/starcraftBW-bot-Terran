#pragma once

#include "MapTools.h"
#include <vector>
#include <queue>
#include <BWAPI.h>
#include <memory>

class ResourceManager {
private:
    static ResourceManager* instance;

    int committedMinerals = 0;
    int committedGas = 0;

    // Constructor privado
    ResourceManager() : committedMinerals(0), committedGas(0) {}

    // Evitar la copia del objeto
    ResourceManager(ResourceManager& other) = delete;
    void operator=(const ResourceManager&) = delete;

public:
    // Método estático para acceder a la instancia
    static ResourceManager* getInstance() {
        if (!instance) {
            instance = new ResourceManager();
        }
        return instance;
    }

    // Métodos para comprometer y liberar recursos
    void commitMinerals(int amount) {
        committedMinerals += amount;
    }

    void releaseMinerals(int amount) {
        committedMinerals -= amount;
    }

    void commitGas(int amount) {
        committedGas += amount;
    }

    void releaseGas(int amount) {
        committedGas -= amount;
    }

    // Métodos para consultar recursos disponibles
    int getAvailableMinerals() const {
        return BWAPI::Broodwar->self()->minerals() - committedMinerals;
    }

    int getAvailableGas() const {
        return BWAPI::Broodwar->self()->gas() - committedGas;
    }

    void onUnitComplete(BWAPI::Unit unit) {
        if (!unit->getType().isBuilding()) { return; }
        releaseMinerals(unit->getType().mineralPrice());
        releaseGas(unit->getType().gasPrice());
    }

    // Destructor
    virtual ~ResourceManager() {
        // Si hay alguna limpieza que hacer cuando el juego termine
        if (instance) {
            delete instance;
            instance = nullptr;
        }
    }


};



class WorkerManager {
private:
	static WorkerManager* instance;

    std::vector<BWAPI::Unit> workers; // Lista de todos los trabajadores
	std::map<BWAPI::Unit, BWAPI::Unit> workerAssignments; // Relaciones de trabajadores con la unidad con la que estan trabajando (ej: mineraField)
    std::map<BWAPI::Unit, BWAPI::TilePosition> workerBuildTargets; // Destinos de construcción para los trabajadores
	void sendIdleWorkersToWork();
	void manageConstructionWorkers();
public:
	// Elimina los métodos de copia
	WorkerManager(WorkerManager& other) = delete;
	void operator=(const WorkerManager&) = delete;
    WorkerManager();
	// Método para acceder a la instancia del Singleton
	static WorkerManager* getInstance();

    // Ejecucion en el juego
	void onStart();
	void onFrame();
	void onUnitCreate(BWAPI::Unit unit);	

	// Asignación y gestión de recursos
    void assignWorkerToMinerals(BWAPI::Unit worker);
    void assignWorkerToGas(BWAPI::Unit worker);
    void balanceWorkerAllocation(); // Rebalancea los trabajadores entre minerales y gas

    // Búsqueda de trabajadores
    BWAPI::Unit getClosestWorkerTo(BWAPI::Position pos);
    BWAPI::Unit getLeastBusyWorker();

    // Gestión de trabajadores en construcción
    void assignWorkerToBuild(BWAPI::Unit worker, BWAPI::UnitType buildingType, BWAPI::TilePosition buildPosition);
    void onBuildingCompletion(BWAPI::Unit building); // Llamado cuando un edificio se completa

    // Defensa y protección
    void protectWorkers();
    void evacuateWorkers(BWAPI::Position dangerZone);

    // Optimización y eficiencia
    void optimizeWorkerEfficiency();

    // Reparación y mantenimiento
    void repairDamagedUnit(BWAPI::Unit unit);

    // Exploración y escaneo del mapa
    void sendWorkerToScout(BWAPI::Position targetPosition);

	// Servicios que WorkerManager ofrece a otras clases
	BWAPI::Unit findBuilder();
	
    // Interfaz y reportes
    std::vector<BWAPI::Unit> getAvailableWorkers();
    void generateWorkerReport();

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
	BWAPI::Unit findTrainingStructure();
};


class BuildOrder
{
	std::queue<std::unique_ptr<Action>> actions;

public:
	void addBuildAction(BWAPI::UnitType unitType, BWAPI::TilePosition buildPosition, int supplyTrigger = -1);
	void addTrainAction(BWAPI::UnitType unitType, int supplyTrigger = -1);
	
	// execute next action on frame
	void onFrame();
	void onStart();
	

};

class StarterBot
{
	MapTools m_mapTools;
	BuildOrder buildOrder;
    bool gameJustStarted;

	// helper functions to get you started with bot programming and learn the API
	void sendIdleWorkersToMinerals(); // Dave
	void trainAdditionalWorkers(); // Dave
	void buildAdditionalSupply(); // Dave
    
	void drawDebugInformation();// modify
	void drawPositionsOfAllUnits(); // 
	void drawResourceManagerInfo();
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



