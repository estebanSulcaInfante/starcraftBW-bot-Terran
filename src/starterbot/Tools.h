#pragma once

#include <BWAPI.h>
#include <set>
// AQUI puse lo que no necesita informacion exacta del bot
namespace Tools
{
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;
    BWAPI::Unit getClosestMineralField(BWAPI::Position pos);
    BWAPI::Unit GetClosestUnitTo(BWAPI::Position p, const BWAPI::Unitset& units);
    BWAPI::Unit GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset& units);

    int CountUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units);
    
    int CountCompletedUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units);
    
    // Retorna un unitset de una cantidad indeterminada de tipos que le pases
    template <class... Types>
    BWAPI::Unitset getUnitsOfTypes(Types... types) {
        
        BWAPI::Unitset unitOfTypes;
        std::set<BWAPI::UnitType> typesSet = { types... };
        // for que recorre todas las unidades del juego con "u"
        for (auto& u : BWAPI::Broodwar->getAllUnits()) {
            // si el tipo esta en tipesets entonces lo agregas
            //set.find si lo encuentra retorna un iterador y si no retorna un .end()
            // equivalente al if variable in lista:
            if (typesSet.find(u->getType()) != typesSet.end()) {
                unitOfTypes.insert(u);
            }
        
        }
        return unitOfTypes;
    }

    BWAPI::Unitset getWorkers();
    BWAPI::Unit GetUnitOfType(BWAPI::UnitType type);
    BWAPI::Unit GetDepot();


    bool BuildBuilding(BWAPI::UnitType type);

    void DrawUnitBoundingBoxes();
    void DrawUnitCommands();
    //void DrawMineralFieldInfo();
    
    
    void SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target);

    int GetTotalSupply(bool inProgress = false);
    
    BWAPI::Unitset getUnitsOnScreen();
    BWAPI::Unitset getUnitsOnScreen(BWAPI::Unitset& units);


    void DrawUnitHealthBars();
    void DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset);
}