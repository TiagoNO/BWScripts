#include "Base.h"
#include <math.h>

int Base::distance(Position a, Position b){
	return int(sqrt(float(((a.x() - b.x()) << 1) + ((a.y() - b.y()) << 1))));
}

Base::Base(Position basePosition) {
	this->buildingList = new BuildingManager();
	this->workers = new WorkerList();
	this->basePosition = basePosition;
}

void Base::update(int mineralBudget, int gasBudget) {

}