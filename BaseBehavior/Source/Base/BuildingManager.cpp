#include "BuildingManager.h"


BuildingManager::BuildingManager() {

}

void BuildingManager::addBuildingList(UnitType buildingType, TilePosition buildingTilePosition,bool priority) {
	UnitItem *unitItem = new UnitItem(buildingType, buildingTilePosition);
	if (priority) {
		this->buildingList.push_front(unitItem);
	}
	else {
		this->buildingList.push_back(unitItem);
	}
	this->onGoingBuildingList[buildingType] = false;
}

void BuildingManager::addBuildingList(UnitType buildingType, TilePosition buildingTilePosition) {
	UnitItem *unitItem = new UnitItem(buildingType, buildingTilePosition);
	this->buildingList.push_back(unitItem);
	this->onGoingBuildingList[buildingType] = false;
}

void BuildingManager::cancelBuilding(UnitType buildingType) {
	for (std::list<UnitItem *>::const_iterator i = this->buildingList.begin(); i != this->buildingList.end(); i++) {
		if ((*i)->getUnitType() == buildingType) {
			this->buildingList.remove((*i));
		}
	}
}

bool BuildingManager::isInBuildingList(UnitType buildingType) {
	for (std::list<UnitItem *>::const_iterator i = this->buildingList.begin(); i != this->buildingList.end(); i++) {
		if ((*i)->getUnitType()== buildingType) {
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

bool BuildingManager::buildFirstInList(Unit *worker){
	if(Broodwar->canBuildHere(worker,this->buildingList.front()->getUnitTilePosition(),this->buildingList.front()->getUnitType(),false)){
		worker->build(this->buildingList.front()->getUnitTilePosition,this->buildingList.front()->getUnitType());
		this->onGoingBuildingList[this->buildingList.front()->getUnitType()] = true;
		this->buildingList.pop_front();
	}
	else{
		// could not build there, for some reason
		Broodwar->sendText("Could not build!");
		this->buildingList.pop_front();
	}
}
