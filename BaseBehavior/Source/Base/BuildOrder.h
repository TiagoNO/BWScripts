#pragma once
#include <set>
#include "BWAPI.h"

#define max_distance 400

using namespace std;

class BuildOrder {
	public:
		virtual void checkMilitaryRequisitions(set<UnitTypes> *requisitions,Position basePosition);
		virtual void checkResourceRequisitions(set<UnitTypes> *requisitions, Position basePosition);
		virtual void checkSpecialRequisitions(set<UnitTypes> *requisitions, Position basePosition);
};


class ZergBuildOrder : public BuildOrder {
	public:
		void checkMilitaryRequisitions(set<UnitTypes> *requisitions, Position basePosition);
		void checkResourceRequisitions(set<UnitTypes> *requisitions, Position basePosition);
		void checkSpecialRequisitions(set<UnitTypes> *requisitions, Position basePosition);
};

class TerranBuildOrder : public BuildOrder {
	public:
		void checkMilitaryRequisitions(set<UnitTypes> *requisitions, Position basePosition);
		void checkResourceRequisitions(set<UnitTypes> *requisitions, Position basePosition);
		void checkSpecialRequisitions(set<UnitTypes> *requisitions, Position basePosition);
};

class ProtossBuildOrder : public BuildOrder {
	public:
		void checkMilitaryRequisitions(set<UnitTypes> *requisitions, Position basePosition);
		void checkResourceRequisitions(set<UnitTypes> *requisitions, Position basePosition);
		void checkSpecialRequisitions(set<UnitTypes> *requisitions, Position basePosition);
};
