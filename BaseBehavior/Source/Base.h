#include "WorkerList.h"
#include "BuildingManager.h"
#include "Requisition.h"
#include "BWAPI.h"

using namespace BWAPI;

class Base{
    private:
        WorkerList *workers;
        Position basePosition;
        BuildingManager *buildingList;

        int distance(Position a,Position b);

    public:
        Base();        
        Base(Position basePosition);

        
}