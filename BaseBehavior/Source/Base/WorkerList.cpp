#include "WorkerList.h"


WorkerList::WorkerList(){
    
}

Unit * WorkerList::getAvaiableWorker(){
    int unconditionalWorkerId = 0;
    for(map<int,int>::const_iterator i = this->workersId.begin(); i != this->workersId.end(); i++){
        if(unconditionalWorkerId == 0 && i->second != is_constructing && i->second != is_scouting){
            unconditionalWorkerId = i->first;
        }
        if(i->second == is_iddle || i->second == is_mining || i->second == is_unknown){
            return Broodwar->getUnit(i->first);
        }
    }
    return Broodwar->getUnit(unconditionalWorkerId);
}

int WorkerList::defineWorkerState(int workerId){
    Unit * worker = Broodwar->getUnit(workerId);
    if(worker->isIdle()){
        return is_iddle;
    }
    else if(worker->isCarryingMinerals() || worker->isCarryingGas()){
        return is_mining;
    }
    else{
        return is_unknown;
    }
}

void WorkerList::addWorker(int workerId){
    int workerState = defineWorkerState(workerId);
    this->workersId[workerId] = workerState;
}

void WorkerList::removeWorker(int workerId){
    this->workersId.erase(workerId);
}

void WorkerList::setWorkerState(int workerId,int state){
    this->workersId[workerId] = state;
}

int WorkerList::getWorkerState(int workerId){
    return this->workersId[workerId];
}

map<int,int> WorkerList::getWorkersId(){
    return this->workersId;
}