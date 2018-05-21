#include "WorkerList.h"

#define max_distance 400

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