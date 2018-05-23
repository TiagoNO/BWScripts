#include "UnitItem.h"

UnitItem::UnitItem(UnitType unitType, Position position) {
	this->unitType = unitType;
	this->unitPosition = position;
}

bool UnitItem::operator==(const UnitItem& a) {
	return this->unitType == a.unitType && this->unitPosition == a.unitPosition;
}

void UnitItem::operator=(const UnitItem& a) {
	this->unitType = a.unitType;
	this->unitPosition = a.unitPosition;
}
