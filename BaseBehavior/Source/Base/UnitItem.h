#pragma once
#include "BWAPI.h"

using namespace BWAPI;

class UnitItem {
	private:
		UnitType unitType;
		TilePosition unitTilePosition;
		
	public:	
		UnitItem(UnitType unitType, TilePosition tilePosition);
		bool operator==(const UnitItem& a);
		void operator=(const UnitItem& a);
		UnitType getUnitType();
		TilePosition getUnitTilePosition();
};