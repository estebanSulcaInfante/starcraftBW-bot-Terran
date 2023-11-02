#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"

StarterBot::StarterBot(){}


// Called when the bot starts!
void StarterBot::onStart()
{
    gameJustStarted = true;

    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(15);
    BWAPI::Broodwar->setFrameSkip(0);
    
    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();
 
    // Obtén la instancia de WorkerManager y llama a onStart
    WorkerManager* workerManager = WorkerManager::getInstance();
    workerManager->onStart();

    // Llama onStart de BuildOrder
    buildOrder.onStart();

    
}


// Called on each frame of the game
void StarterBot::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();    

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
    
    
    // Llama al onFrame del WorkerManager
    WorkerManager* workerManager = WorkerManager::getInstance();
    workerManager->onFrame();
    
    // Llama a onFrame de BuildOrder
    buildOrder.onFrame();

    gameJustStarted = false;
}


// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle())
        {
            // Get the closest mineral to this worker unit
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral) { unit->rightClick(closestMineral); }
        }
    }
}


// Train more workers so we can gather more income
void StarterBot::trainAdditionalWorkers() 
{
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const int workersWanted = 20;
    const int workersOwned = Tools::CountCompletedUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    if (workersOwned < workersWanted)
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = Tools::GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
    }
}


// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 2) { return; }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    const bool startedBuilding = Tools::BuildBuilding(supplyProviderType);
    if (startedBuilding)
    {
        BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
    }
}


// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{
    drawPositionsOfAllUnits();
    drawResourceManagerInfo();
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Dibuja la informacion del ResourceManager
void StarterBot::drawResourceManagerInfo()
{
    BWAPI::Position pos = BWAPI::Position(2, 2);
    // Parse
    ResourceManager* resourceManager = ResourceManager::getInstance();
    std::string info = "mineral: " + std::to_string(resourceManager->getAvailableMinerals()) + "\n"
        "gas: " + std::to_string(resourceManager->getAvailableGas());

    // Draw

    BWAPI::Broodwar->drawTextScreen(pos, info.c_str());
}


// Dibuja las Position y Tile Position de todas las unidades
void StarterBot::drawPositionsOfAllUnits()
{
    BWAPI::Unitset units = BWAPI::Broodwar->getAllUnits();
    for (auto unit : units)
    {
        // Obtener todas las posiciones
        BWAPI::Position position = unit->getPosition();
        BWAPI::TilePosition tilePosition = unit->getTilePosition();
        
        // Parse
        std::string info = "(" + std::to_string(position.x) + "," + std::to_string(position.y) + ") " +
            "Tile: (" + std::to_string(tilePosition.x) + "," + std::to_string(tilePosition.y) + ")";

        // Draw
        BWAPI::Broodwar->drawTextMap(position, info.c_str());
                
    }

}


// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner)
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}


// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
	
}


// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{
	
}


// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text) 
{ 
    
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }
    else if (text == "hola")
    {
        BWAPI::Broodwar->sendText("mundo:)");
    }
}


// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{ 
    
	// Llama al metodo onUnitCreate del WorkerManager
    WorkerManager* workerManager = WorkerManager::getInstance();
    workerManager->onUnitCreate(unit);
    
    // Evitar que se cuente los recursos de las unidades creadas al inicio de la partida
    if (!gameJustStarted) 
    {
        ResourceManager::getInstance()->onUnitComplete(unit);
    }
    
}


// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{
	
}


// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{ 
	
}


// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{ 
	
}


// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{ 
	
}
// ************************ResourceManager**********************************
ResourceManager* ResourceManager::instance = nullptr;






// ************************WorkerManager**********************************
// Inicializa la instancia del Singleton
WorkerManager* WorkerManager::instance = nullptr;

WorkerManager* WorkerManager::getInstance()
{
    if (instance == nullptr) {
        instance = new WorkerManager();
    }
    return instance;
}

WorkerManager::WorkerManager() {}

void WorkerManager::onStart() {
    // Asegurarse de que las listas y mapas están vacíos.
    workers.clear();
    workerAssignments.clear();
    workerBuildTargets.clear();

    // Identificar y almacenar los trabajadores iniciales.
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->getType().isWorker()) {
            workers.push_back(unit);
            // Por defecto, asumiremos que todos los trabajadores deberían estar recolectando minerales.
            assignWorkerToMinerals(unit);
        }
    }
    
    // Otras configuraciones iniciales, si son necesarias.
}

void WorkerManager::assignWorkerToGas(BWAPI:: Unit worker)
{

}

void WorkerManager::assignWorkerToMinerals(BWAPI::Unit worker) {
    // Encontrar el parche de mineral más cercano
    BWAPI::Unit mineralPatch = Tools::getClosestMineralField(worker->getPosition());

    if (mineralPatch) {
        // Asignar el trabajador al parche de mineral
        workerAssignments[worker] = mineralPatch;

        // Ordenar al trabajador que recolecte minerales
        worker->gather(mineralPatch);
    }
}

void WorkerManager::balanceWorkerAllocation()
{

}

void WorkerManager::sendIdleWorkersToWork()
{
    // Iterar sobre todos los trabajadores para asegurarse de que están haciendo algo útil.
    for (auto& worker : workers) {
        if (worker->isIdle()) {
            // Si el trabajador está ocioso, asignarlo a recolectar minerales.
            assignWorkerToMinerals(worker);
        }
        else {
            // Aqui podría agregar lógica adicional para manejar otros casos, como trabajadores que están construyendo, recolectando gas, etc.
        }
    }
}

void WorkerManager::manageConstructionWorkers() {
    for (auto& [worker, buildTarget] : workerBuildTargets) {
        if (worker->isConstructing()) {
            // Si el trabajador está construyendo, asegurarte de que continúe construyendo.
            // Aqui podria agregar lógica para manejar casos en los que la construcción se interrumpa, etc.
        }
        else {
            // Si el trabajador no está construyendo, enviarlo al lugar de construcción.
            //worker->move(BWAPI::Position(buildTarget));
        }
    }
}

BWAPI::Unit WorkerManager::getLeastBusyWorker()
{
    return nullptr;
}

void WorkerManager::assignWorkerToBuild(BWAPI::Unit worker, BWAPI::UnitType buildingType, BWAPI::TilePosition buildPosition) {
    // Verificar si el trabajador es válido y es un trabajador
    if (!worker || !worker->getType().isWorker()) {
        return;
    }

    // Asignar al trabajador la tarea de construcción
    workerBuildTargets[worker] = buildPosition;

    // Ordenar al trabajador que construya
    worker->build(buildingType, buildPosition);
}


void WorkerManager::onBuildingCompletion(BWAPI::Unit building)
{

}

void WorkerManager::protectWorkers()
{

}

void WorkerManager::evacuateWorkers(BWAPI::Position dangerZone)
{

}


void WorkerManager::optimizeWorkerEfficiency()
{

}

void WorkerManager::repairDamagedUnit(BWAPI::Unit unit)
{

}

void WorkerManager::sendWorkerToScout(BWAPI::Position targetPosition)
{

}

std::vector<BWAPI::Unit> WorkerManager::getAvailableWorkers()
{
    return std::vector<BWAPI::Unit>();
}

void WorkerManager::generateWorkerReport()
{

}

void WorkerManager::onFrame() {
    // Mandar a los trabajadores que no hacen nada a los minerales
    sendIdleWorkersToWork();

    // Verificar si necesitas equilibrar la asignación de trabajadores entre minerales y gas.
    balanceWorkerAllocation();

    // Gestionar trabajadores en construcción.
    manageConstructionWorkers();

    // Otras tareas específicas en cada frame
    protectWorkers();
    optimizeWorkerEfficiency();

    // Generar informes si es necesario (puede ser útil para depuración).
    generateWorkerReport();
}

BWAPI::Unit WorkerManager::findBuilder() {
    // Obtener todos los trabajadores
    BWAPI::Unitset workers = BWAPI::Broodwar->self()->getUnits();
    BWAPI::Unit builder = nullptr;

    // Buscar un trabajador que esté minando minerales
    for (auto& worker : workers) {
        if (worker->getType().isWorker() && worker->isGatheringMinerals()) {
            builder = worker;
            break;
        }
    }

    return builder;
}

void WorkerManager::onUnitCreate(BWAPI::Unit unit) {
    // Verificar si la unidad es un trabajador y pertenece al jugador actual
    if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self()) {
        workers.push_back(unit);
    }
}


// ************************Action**********************************



Action::Action(BWAPI::UnitType type, int supplyTrigger)
    : type(type), supplyTrigger(supplyTrigger),
    mineralCost(type.mineralPrice()), 
    gasCost(type.gasPrice()) { }


int Action::getSupplyTrigger() { return supplyTrigger; }

int Action::getMineralCost() { return mineralCost; }

int Action::getGasCost() { return gasCost; }


// **************************BuildAction******************************

BuildAction::BuildAction::BuildAction(BWAPI::UnitType type, int supplyTrigger, BWAPI::TilePosition buildPosition)
    : Action(type, supplyTrigger), buildPosition(buildPosition) {
    
    // Calcular el costo de minerales y gas a partir del tipo de unidad.
    mineralCost = type.mineralPrice();
    gasCost = type.gasPrice();
}

bool BuildAction::canExecute()
{
    // Instanciar el ResourceManager
    ResourceManager* resourceManager = ResourceManager::getInstance();

    // Verificar recursos
    if (resourceManager->getAvailableMinerals() < mineralCost ||
        resourceManager->getAvailableGas() < gasCost ||
        BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed() < type.supplyRequired()) {
        return false;
    }

    // Verificar disponibilidad de trabajador
    //WorkerManager* workerManager = WorkerManager::getInstance();
    //BWAPI::Unit builder = workerManager->findBuilder();
    //if (!builder) {
    //    return false;
    //}

    // Verificar posición de construcción (y cualquier otra condición necesaria)
    // ...

    return true;
}

void BuildAction::execute() 
{

    // Obtener un gestor de trabajadores
    WorkerManager* workerManager = WorkerManager::getInstance();

    // Encontrar un trabajador para construir
    BWAPI::Unit builder = workerManager->findBuilder();
    if (!builder) {
        BWAPI::Broodwar->printf("Error: No se encontró un trabajador para construir");
        return;
    }

    // Verificar si el lugar de construcción es válido
    BWAPI::TilePosition buildTile = buildPosition;
    if (!BWAPI::Broodwar->canBuildHere(buildTile, type, builder)) {
        BWAPI::Broodwar->printf("Error: No se puede construir aquí");
        return;
    }

    // Mover al trabajador a la posición de construcción y comenzar a construir
    bool constructionStarted = builder->build(type, buildTile);
    if (!constructionStarted) {
        BWAPI::Broodwar->printf("Error: La construcción no pudo comenzar");
        return;
    }


    // Obtener el ResourceManager
    ResourceManager* resourceManager = ResourceManager::getInstance();

    // Descontar los recursos utilizados
    resourceManager->commitMinerals(this->getMineralCost());
    resourceManager->commitGas(this->getGasCost());

    // Asignar el trabajador a la tarea de construcción
    workerManager->assignWorkerToBuild(builder, type, buildTile);

}

// ***************************TrainAction***************************


TrainAction::TrainAction(BWAPI::UnitType type, int supplyTrigger)
    : Action(type, supplyTrigger) { }


BWAPI::Unit TrainAction::findTrainingStructure() {
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->getType().isBuilding() && unit->isIdle() && unit->canTrain(type)) {
            return unit;
        }
    }
    return nullptr;
}

bool TrainAction::canExecute() // los can execute y execute funcionan porque se ejecutan juntos, pero si es ejecutaran en distintos frames fallarian
{
    // Verificar recursos
    if (BWAPI::Broodwar->self()->minerals() < mineralCost ||
        BWAPI::Broodwar->self()->gas() < gasCost ||
        BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed() < type.supplyRequired()) {
        return false;
    }

    // Verificar si existe alguna estructura adecuada y disponible
    BWAPI::Unit structure = findTrainingStructure();
    if (!structure) {
        return false;
    }

    // Cualquier otra condición necesaria
    // ...

    return true;
}

void TrainAction::execute()
{
    if (!canExecute()) { return; }

    // Encontrar una estructura de entrenamiento adecuada
    BWAPI::Unit structure = findTrainingStructure();
    if (!structure) {
        return; // Si no encontramos una estructura, simplemente retornamos
    }

    // Iniciar la producción de la unidad
    bool startedTraining = structure->train(type);

    if (startedTraining) {
        BWAPI::Broodwar->printf("Entrenando una unidad de tipo: %s", type.c_str());
    }
    else {
        BWAPI::Broodwar->printf("Error al intentar entrenar una unidad de tipo: %s", type.c_str());
    }
}


// ****************************BuildOrder**************************


void BuildOrder::addBuildAction(BWAPI::UnitType unitType, BWAPI::TilePosition buildPosition, int supplyTrigger )
{
    // si no le pasas un SupplyTrigger, se pondra por defecto la cantidad de Supplies que se necesitan para
    // ese tipo de unidad.
    if (supplyTrigger == -1) {
        supplyTrigger = BWAPI::Broodwar->self()->supplyUsed() / 2;
    }
    
    actions.push(std::make_unique<BuildAction>(unitType, supplyTrigger, buildPosition));
}

void BuildOrder::addTrainAction(BWAPI::UnitType unitType, int supplyTrigger)
{
    // Si no le pasas un SupplyTrigger, se pondra por defecto la cantidad de Supplies en ese momento
    if (supplyTrigger == -1) {
        supplyTrigger = BWAPI::Broodwar->self()->supplyUsed() / 2;
    }
 
    actions.push(std::make_unique<TrainAction>(unitType, supplyTrigger));
}


void BuildOrder::onFrame()
{
    // Controlar que la cola no este vacia, para evitar comportamientos indefinidos
    if (actions.empty()) { return; }
    
    auto& nextAction = actions.front();
    int currentSupply = BWAPI::Broodwar->self()->supplyUsed() / 2;  // Dividido por 2 porque BWAPI devuelve el doble del valor real
    // Verificar recursos y ejecutar acción. 
    if (nextAction->canExecute()) {
        nextAction->execute();
        actions.pop();
    }
}

void BuildOrder::onStart()
{
    // Asumiendo que basePosition es la posición donde quieres construir tus estructuras
    BWAPI::TilePosition basePosition = BWAPI::Broodwar->self()->getStartLocation(); // Establece tu posición inicial aquí
    basePosition = BWAPI::Broodwar->getBuildLocation(BWAPI::UnitTypes::Terran_Barracks, basePosition);
    // Construir SCV (4 veces para tener 4 SCVs adicionales)
    for (int i = 0; i < 4; ++i) {
        addTrainAction(BWAPI::UnitTypes::Terran_SCV, -1);  // El -1 significa que no hay un trigger de suministro específico
    }

    // Construir refineria de vespeno
    BWAPI::TilePosition startPosition = BWAPI::Broodwar->self()->getStartLocation(); // Obtener la posición inicial
    BWAPI::Unitset vespeneGeysers = Tools::getUnitsOfTypes(BWAPI::UnitTypes::Resource_Vespene_Geyser);
    BWAPI::Unit closestVespeneGeyser = Tools::GetClosestUnitTo(BWAPI::Position(startPosition), vespeneGeysers);
    if (closestVespeneGeyser) {
        BWAPI::TilePosition vespeneBuildPos = closestVespeneGeyser->getTilePosition();
        addBuildAction(BWAPI::UnitTypes::Terran_Refinery, vespeneBuildPos);
    }


    // Construir un Supply Depot después del cuarto SCV
    addBuildAction(BWAPI::UnitTypes::Terran_Supply_Depot, basePosition, 8 );  // Este se inicia cuando llegas a 8 de suministro


    // Construir SCV (2)
    for (int i = 0; i < 2; ++i) {
        addTrainAction(BWAPI::UnitTypes::Terran_SCV, -1);  // El -1 significa que no hay un trigger de suministro específico
    }

    // Construir un Barracks después del Supply Depot
    basePosition.y += 2;
    addBuildAction(BWAPI::UnitTypes::Terran_Barracks, basePosition, 10);

    // Construir un Barracks después del Supply Depot
    basePosition.y += 2;
    addBuildAction(BWAPI::UnitTypes::Terran_Barracks, basePosition, 10);

    // Continuar construyendo SCVs
    for (int i = 0; i < 5; ++i) {
        addTrainAction(BWAPI::UnitTypes::Terran_SCV, -1);
    }

    basePosition.y += 2;
    // Construir otro Supply Depot para aumentar el suministro
    addBuildAction(BWAPI::UnitTypes::Terran_Supply_Depot,basePosition);
 
    basePosition.y += 2;
    // Construir un Academy después de tener suficientes SCVs
    addBuildAction(BWAPI::UnitTypes::Terran_Academy,basePosition);
 
    basePosition.y -= 15;
    // Construir otro Supply Depot para aumentar el suministro
    addBuildAction(BWAPI::UnitTypes::Terran_Supply_Depot,basePosition);
 
    basePosition.y -= 2;
    // Construir otro Supply Depot para aumentar el suministro
    addBuildAction(BWAPI::UnitTypes::Terran_Supply_Depot,basePosition);
    
    // Entrenar 10 Marines
    for (int i = 0; i < 10; ++i) {
        addTrainAction(BWAPI::UnitTypes::Terran_Marine, -1);
    }

    // Entrenar 2 Medics
    for (int i = 0; i < 2; ++i) {
        addTrainAction(BWAPI::UnitTypes::Terran_Medic, -1);
    }
}