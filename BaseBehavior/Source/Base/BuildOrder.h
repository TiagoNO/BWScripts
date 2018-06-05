#pragma once
#include <set>
#include "BWAPI.h"
#include "BWAPI.h"
#define max_distance 400

using namespace std;
using namespace BWAPI;

enum Max_Unit_Number{
	Protoss_Gateway = 5,
	Protoss_Cybernetics_Core = 1,
};

class BuildOrder {
	public:
		virtual void checkMilitaryRequisitions(set<UnitType> *requisitions,Position basePosition);
		virtual void checkResourceRequisitions(set<UnitType> *requisitions, Position basePosition);
		virtual void checkSpecialRequisitions(set<UnitType> *requisitions, Position basePosition);
};


class ZergBuildOrder : public BuildOrder {
	public:
		void checkMilitaryRequisitions(set<UnitType> *requisitions, Position basePosition);
		void checkResourceRequisitions(set<UnitType> *requisitions, Position basePosition);
		void checkSpecialRequisitions(set<UnitType> *requisitions, Position basePosition);
};

class TerranBuildOrder : public BuildOrder {
	public:
		void checkMilitaryRequisitions(set<UnitType> *requisitions, Position basePosition);
		void checkResourceRequisitions(set<UnitType> *requisitions, Position basePosition);
		void checkSpecialRequisitions(set<UnitType> *requisitions, Position basePosition);
};

class ProtossBuildOrder : public BuildOrder {
	public:
		void checkMilitaryRequisitions(set<UnitType> *requisitions, Position basePosition);
		void checkResourceRequisitions(set<UnitType> *requisitions, Position basePosition);
		void checkSpecialRequisitions(set<UnitType> *requisitions, Position basePosition);
};
