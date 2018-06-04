#include "UnitItem.h"

UnitItem::UnitItem(UnitType unitType, TilePosition tilePosition) {
	this->unitType = unitType;
	this->unitTilePosition = tilePosition;
}

bool UnitItem::operator==(const UnitItem& a) {
	return this->unitType == a.unitType && this->unitTilePosition == a.unitTilePosition;
}

void UnitItem::operator=(const UnitItem& a) {
	this->unitType = a.unitType;
	this->unitTilePosition = a.unitTilePosition;
}

UnitType UnitItem::getUnitType(){
	return this->unitType;
}
TilePosition UnitItem::getUnitTilePosition(){
	return this->unitTilePosition;
}
