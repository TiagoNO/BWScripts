#include "BuildingManager.h"


BuildingManager::BuildingManager() {

}

void BuildingManager::addBuildingList(UnitType buildingType, Position buildingPosition,bool priority) {
	UnitItem *unitItem = new UnitItem(buildingType, buildingPosition);
	if (priority) {
		this->buildingList.push_front(unitItem);
	}
	else {
		this->buildingList.push_back(unitItem);
	}
	this->onGoingBuildingList[buildingType] = false;
}

void BuildingManager::addBuildingList(UnitType buildingType, Position buildingPosition) {
	UnitItem *unitItem = new UnitItem(buildingType, buildingPosition);
	this->buildingList.push_back(unitItem);
	this->onGoingBuildingList[buildingType] = false;
}

void BuildingManager::cancelBuilding(UnitType buildingType) {
	for (std::list<UnitItem *>::const_iterator i = this->buildingList.begin(); i != this->buildingList.end(); i++) {
		if ((*i)->unitType == buildingType) {
			this->buildingList.remove((*i));
		}
	}
}

bool BuildingManager::isInBuildingList(UnitType buildingType) {
	for (std::list<UnitItem *>::const_iterator i = this->buildingList.begin(); i != this->buildingList.end(); i++) {
		if ((*i)->unitType == buildingType) {
			return true;
		}
	}
	return false;
}

bool BuildingManager::isBeeingBuilt(UnitType buildingType) {
	if (this->onGoingBuildingList.find(buildingType) != this->onGoingBuildingList.end()) {
		return true;
	}
	return false;
}
