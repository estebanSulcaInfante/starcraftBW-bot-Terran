#include "Tools.h"
#include "string"


BWAPI::Unit Tools::GetClosestUnitTo(BWAPI::Position p, const BWAPI::Unitset& units)
{
    BWAPI::Unit closestUnit = nullptr;

    for (auto& u : units)
    {
        if (!closestUnit || u->getDistance(p) < closestUnit->getDistance(p))
        {
            closestUnit = u;
        }
    }

    return closestUnit;
}

BWAPI::Unit Tools::getClosestMineralField(BWAPI::Position pos) {
    BWAPI::Unit closestMineralField = nullptr;
    double closestDistance = std::numeric_limits<double>::infinity();

    // Obtener todos los campos de minerales
    for (auto& mineralField : BWAPI::Broodwar->getMinerals()) {
        // Calcular la distancia entre la posici�n dada y el campo de mineral
        double distance = pos.getDistance(mineralField->getPosition());

        // Actualizar el campo de mineral m�s cercano si es necesario
        if (distance < closestDistance) {
            closestMineralField = mineralField;
            closestDistance = distance;
        }
    }

    // Devolver el campo de mineral m�s cercano
    return closestMineralField;
}

BWAPI::Unit Tools::GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset& units)
{
    if (!unit) { return nullptr; }
    return GetClosestUnitTo(unit->getPosition(), units);
}

int Tools::CountUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units)
{
    int sum = 0;
    for (auto& unit : units)
    {
        if (unit->getType() == type)
        {
            sum++;
        }
    }

    return sum;
}

// Count just units already created, not in progress
int Tools::CountCompletedUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units)
{
    int sum = 0;
    for (auto& unit : units)
    {
        if (unit->getType() == type && unit->isCompleted())
        {
            sum++;
        }
    }
    return sum;
}



BWAPI::Unit Tools::GetUnitOfType(BWAPI::UnitType type)
{
    // For each unit that we own
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // if the unit is of the correct type, and it actually has been constructed, return it
        if (unit->getType() == type && unit->isCompleted())
        {
            return unit;
        }
    }

    // If we didn't find a valid unit to return, make sure we return nullptr
    return nullptr;
}

// get all workers of player
BWAPI::Unitset Tools::getWorkers() {
    BWAPI::Unitset workers;
    for (auto& u : BWAPI::Broodwar->self()->getUnits()) {
        if (u->getType().isWorker()) {
            workers.insert(u);
        }
    }
    return workers;
}

BWAPI::Unit Tools::GetDepot()
{
    const BWAPI::UnitType depot = BWAPI::Broodwar->self()->getRace().getResourceDepot();
    return GetUnitOfType(depot);
}

// Get units on screen filter on the Unitset
BWAPI::Unitset Tools::getUnitsOnScreen(BWAPI::Unitset& units) {
    BWAPI::Unitset unitsOnScreen;

    BWAPI::Position screenTopLeft = BWAPI::Broodwar->getScreenPosition();
    BWAPI::Position screenBottomRight = screenTopLeft + BWAPI::Position(
        Tools::SCREEN_WIDTH, Tools::SCREEN_HEIGHT);

    for (auto& unit : units) {
        BWAPI::Position unitPos = unit->getPosition();
        BWAPI::UnitType type = unit->getType();

        // Calcular los l�mites de la unidad
        int left = unitPos.x - type.dimensionLeft();
        int right = unitPos.x + type.dimensionRight();
        int top = unitPos.y - type.dimensionUp();
        int bottom = unitPos.y + type.dimensionDown();

        // Verificar si la unidad est� en la pantalla
        if (right >= screenTopLeft.x && left <= screenBottomRight.x &&
            bottom >= screenTopLeft.y && top <= screenBottomRight.y) {
            unitsOnScreen.insert(unit);
        }
    }

    return unitsOnScreen;
}
// Get all units on screen
BWAPI::Unitset Tools::getUnitsOnScreen() {
    BWAPI::Unitset unitsOnScreen;

    BWAPI::Position screenTopLeft = BWAPI::Broodwar->getScreenPosition();
    BWAPI::Position screenBottomRight = screenTopLeft + BWAPI::Position(
        Tools::SCREEN_WIDTH, Tools::SCREEN_HEIGHT);

    for (auto& unit : BWAPI::Broodwar->getAllUnits()) {
        BWAPI::Position unitPos = unit->getPosition();
        BWAPI::UnitType type = unit->getType();

        // Calcular los l�mites de la unidad
        int left = unitPos.x - type.dimensionLeft();
        int right = unitPos.x + type.dimensionRight();
        int top = unitPos.y - type.dimensionUp();
        int bottom = unitPos.y + type.dimensionDown();

        // Verificar si la unidad est� en la pantalla
        if (right >= screenTopLeft.x && left <= screenBottomRight.x &&
            bottom >= screenTopLeft.y && top <= screenBottomRight.y) {
            unitsOnScreen.insert(unit);
        }
    }

    return unitsOnScreen;
}

// Attempt tp construct a building of a given type 
bool Tools::BuildBuilding(BWAPI::UnitType type)
{
    // Get the type of unit that is required to build the desired building
    BWAPI::UnitType builderType = type.whatBuilds().first;

    // Get a unit that we own that is of the given type so it can build
    // If we can't find a valid builder unit, then we have to cancel the building
    BWAPI::Unit builder = Tools::GetUnitOfType(builderType);
    if (!builder) { return false; }

    // Get a location that we want to build the building next to
    BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation();

    // Ask BWAPI for a building location near the desired position for the type
    int maxBuildRange = 64;
    bool buildingOnCreep = type.requiresCreep();
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(type, desiredPos, maxBuildRange, buildingOnCreep);
    return builder->build(type, buildPos);
}

void Tools::DrawUnitCommands()
{
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        const BWAPI::UnitCommand & command = unit->getLastCommand();

        // If the previous command had a ground position target, draw it in red
        // Example: move to location on the map
        if (command.getTargetPosition() != BWAPI::Positions::None)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTargetPosition(), BWAPI::Colors::Red);
        }

        // If the previous command had a tile position target, draw it in red
        // Example: build at given tile position location
        if (command.getTargetTilePosition() != BWAPI::TilePositions::None)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), BWAPI::Position(command.getTargetTilePosition()), BWAPI::Colors::Green);
        }

        // If the previous command had a unit target, draw it in red
        // Example: attack unit, mine mineral, etc
        if (command.getTarget() != nullptr)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTarget()->getPosition(), BWAPI::Colors::White);
        }
    }
}


void Tools::DrawUnitBoundingBoxes()
{
    for (auto& unit : BWAPI::Broodwar->getAllUnits())
    {
        BWAPI::Position topLeft(unit->getLeft(), unit->getTop());
        BWAPI::Position bottomRight(unit->getRight(), unit->getBottom());
        BWAPI::Broodwar->drawBoxMap(topLeft, bottomRight, BWAPI::Colors::White);
    }
}

//void Tools::DrawMineralFieldInfo()
//{
//    // Iterate between all Units
//    for (auto& u : BWAPI::Broodwar->getAllUnits()) {
//        // Filter for Mineral
//        if (u->getType() == BWAPI::UnitTypes::Resource_Mineral_Field ||
//            u->getType() == BWAPI::UnitTypes::Resource_Mineral_Field_Type_2 ||
//            u->getType() == BWAPI::UnitTypes::Resource_Mineral_Field_Type_3) 
//        {
//            // Get Mineral Field info
//            BWAPI::Position position = u->getPosition(); // position
//            int resources = u->getResources(); // actual resources
//            
//            // Convert integer to string and concatenate
//            std::string info = "Minerals: " + std::to_string(resources);
//            
//            // Draw the text on the map
//            BWAPI::Broodwar->drawTextMap(position, info.c_str());
//        }
//    }    
//}


void Tools::SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target)
{
    // if there's no valid unit, ignore the command
    if (!unit || !target) { return; }

    // Don't issue a 2nd command to the unit on the same frame
    if (unit->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount()) { return; }

    // If we are issuing the same type of command with the same arguments, we can ignore it
    // Issuing multiple identical commands on successive frames can lead to bugs
    if (unit->getLastCommand().getTarget() == target) { return; }
    
    // If there's nothing left to stop us, right click!
    unit->rightClick(target);
}

int Tools::GetTotalSupply(bool inProgress)
{
    // start the calculation by looking at our current completed supplyt
    int totalSupply = BWAPI::Broodwar->self()->supplyTotal();

    // if we don't want to calculate the supply in progress, just return that value
    if (!inProgress) { return totalSupply; }

    // if we do care about supply in progress, check all the currently constructing units if they will add supply
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // ignore units that are fully completed
        if (unit->isCompleted()) { continue; }

        // if they are not completed, then add their supply provided to the total supply
        totalSupply += unit->getType().supplyProvided();
    }

    // one last tricky case: if a unit is currently on its way to build a supply provider, add it
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // get the last command given to the unit
        const BWAPI::UnitCommand& command = unit->getLastCommand();

        // if it's not a build command we can ignore it
        if (command.getType() != BWAPI::UnitCommandTypes::Build) { continue; }

        // add the supply amount of the unit that it's trying to build
        totalSupply += command.getUnitType().supplyProvided();
    }

    return totalSupply;
}

void Tools::DrawUnitHealthBars()
{
    // how far up from the unit to draw the health bar
    int verticalOffset = -10;

    // draw a health bar for each unit on the map
    for (auto& unit : BWAPI::Broodwar->getAllUnits())
    {
        // determine the position and dimensions of the unit
        const BWAPI::Position& pos = unit->getPosition();
        int left = pos.x - unit->getType().dimensionLeft();
        int right = pos.x + unit->getType().dimensionRight();
        int top = pos.y - unit->getType().dimensionUp();
        int bottom = pos.y + unit->getType().dimensionDown();

        // if it's a resource, draw the resources remaining
        if (unit->getType().isResourceContainer() && unit->getInitialResources() > 0)
        {
            double mineralRatio = (double)unit->getResources() / (double)unit->getInitialResources();
            DrawHealthBar(unit, mineralRatio, BWAPI::Colors::Cyan, 0);
        }
        // otherwise if it's a unit, draw the hp 
        else if (unit->getType().maxHitPoints() > 0)
        {
            double hpRatio = (double)unit->getHitPoints() / (double)unit->getType().maxHitPoints();
            BWAPI::Color hpColor = BWAPI::Colors::Green;
            if (hpRatio < 0.66) hpColor = BWAPI::Colors::Orange;
            if (hpRatio < 0.33) hpColor = BWAPI::Colors::Red;
            DrawHealthBar(unit, hpRatio, hpColor, 0);
            
            // if it has shields, draw those too
            if (unit->getType().maxShields() > 0)
            {
                double shieldRatio = (double)unit->getShields() / (double)unit->getType().maxShields();
                DrawHealthBar(unit, shieldRatio, BWAPI::Colors::Blue, -3);
            }
        }
    }
}

void Tools::DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset)
{
    int verticalOffset = -10;
    const BWAPI::Position& pos = unit->getPosition();

    int left = pos.x - unit->getType().dimensionLeft();
    int right = pos.x + unit->getType().dimensionRight();
    int top = pos.y - unit->getType().dimensionUp();
    int bottom = pos.y + unit->getType().dimensionDown();

    int ratioRight = left + (int)((right - left) * ratio);
    int hpTop = top + yOffset + verticalOffset;
    int hpBottom = top + 4 + yOffset + verticalOffset;

    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), color, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

    int ticWidth = 3;

    for (int i(left); i < right - 1; i += ticWidth)
    {
        BWAPI::Broodwar->drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
    }
}