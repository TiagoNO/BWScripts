#include "WorkerList.h"
#include "BuildingManager.h"
#include "BWAPI.h"

#define max_distance 400

using namespace BWAPI;

class Base{
    private:
        WorkerList *workers;
        Position basePosition;
        BuildingManager *buildingList;
		Requisition *baseRequisitions;

        int distance(Position a,Position b);

    public:
        Base();        
        Base(Position basePosition);

        
}