#include "Requisition.h"

#include "BWAPI.h"

#define max_distance 400

Requisition::Requisition() {
	if (Broodwar->self()->getRace() == Races::Protoss) {
		this->buildOrder = new ProtossBuildOrder();
	}
	else if (Broodwar->self()->getRace() == Races::Terran) {
		this->buildOrder = new TerranBuildOrder();
	}
	else if (Broodwar->self()->getRace() == Races::Zerg) {
		this->buildOrder = new ZergBuildOrder();
	}
}

Requisition* Requisition::getInstance() {
	if (this->instance == NULL) {
		this->instance = new Requisition();
	}
	return this->instance;
}

set<UnitType> Requisition::getRequisitions(Position basePosition) {
	set<UnitTypes> requisitions;
	this->buildOrder->checkMilitaryRequisitions(requisitions);
	this->buildOrder->checkResourceRequisitions(requisitions);
	this->buildOrder->checkSpecialRequisitions(requisitions);
	return requisitions;
}
