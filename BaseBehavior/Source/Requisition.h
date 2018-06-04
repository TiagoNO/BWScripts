#include "BuildOrder.h"
#include <set>

using namespace std;

#pragma once
class Requisition {
	private:
		Requisition *instance;
		BuildOrder *buildOrder;

	public:
		Requisition();

		Requisition* getInstance();

		set<UnitType> getRequisitions(Position basePosition);

};