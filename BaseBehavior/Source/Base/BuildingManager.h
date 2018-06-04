#pragma once
#include "BWAPI.h"
#include "UnitItem.h"

class BuildingManager {
	private:
		std::list<UnitItem*> buildingList;
		std::map<UnitType, bool> onGoingBuildingList;
	public:
		BuildingManager();

		void addBuildingList(UnitType buildingType, TilePosition buildingTilePosition,bool priority);
		void addBuildingList(UnitType buildingType, TilePosition buildingTilePosition);

		void cancelBuilding(UnitType buildingType);

		bool isInBuildingList(UnitType buildingType);
		bool isBeeingBuilt(UnitType buildingType);

		bool buildFirstInList(Unit *worker);
};