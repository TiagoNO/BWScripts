#pragma once
#include "BWAPI.h"

using namespace BWAPI;

class UnitItem {
	public:
		UnitType unitType;
		Position unitPosition;
		
		
		UnitItem(UnitType unitType, Position position);
		bool operator==(const UnitItem& a);
		void operator=(const UnitItem& a);
};