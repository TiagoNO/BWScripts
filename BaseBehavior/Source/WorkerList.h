//#include "BWAPI.h"
#include <map>

using namespace std;
 
#define is_iddle 0
#define is_constructing 1
#define is_mining 2
#define is_scouting 3
#define is_unknown 4

class WorkerList{
private:
    map<int,int> workersId;

    int defineWorkerState(int workerId);

public:
    WorkerList();

    /*get the closest*/
    Unit* getAvaiableWorker();

    /*add an worker in the list*/
    void addWorker(int workerId);

    /*remove an worker from the list*/
    void removeWorker(int workerId);    

    /*set worker with an certain state*/
    void setWorkerState(int workerId,int state);

    /*return the state of the worker*/
    int getWorkerState(int workerId);

    map<int,int> getWorkersId();
}