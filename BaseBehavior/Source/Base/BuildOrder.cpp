#include "BuildOrder.h"


void ZergBuildOrder::checkMilitaryRequisitions(set<UnitType> *requisitions, Position basePosition) {
	for (std::set<Unit *>::const_iterator i = Broodwar->getUnitsInRadius(basePosition, max_distance).begin(); i != Broodwar->getUnitsInRadius(basePosition,max_distance).end(); i++){

	}
}

void ZergBuildOrder::checkResourceRequisitions(set<UnitType> *requisitions, Position basePosition) {
	for (std::set<Unit *>::const_iterator i = Broodwar->getUnitsInRadius(basePosition, max_distance).begin(); i != Broodwar->getUnitsInRadius(basePosition, max_distance).end(); i++) {

	}
}

void ZergBuildOrder::checkSpecialRequisitions(set<UnitType> *requisitions, Position basePosition) {
	for (std::set<Unit *>::const_iterator i = Broodwar->getUnitsInRadius(basePosition, max_distance).begin(); i != Broodwar->getUnitsInRadius(basePosition, max_distance).end(); i++) {

	}
}





void TerranBuildOrder::checkMilitaryRequisitions(set<UnitType> *requisitions, Position basePosition) {
	for (std::set<Unit *>::const_iterator i = Broodwar->getUnitsInRadius(basePosition, max_distance).begin(); i != Broodwar->getUnitsInRadius(basePosition, max_distance).end(); i++) {

	}
}

void TerranBuildOrder::checkResourceRequisitions(set<UnitType> *requisitions, Position basePosition) {
	for (std::set<Unit *>::const_iterator i = Broodwar->getUnitsInRadius(basePosition, max_distance).begin(); i != Broodwar->getUnitsInRadius(basePosition, max_distance).end(); i++) {

	}
}

void TerranBuildOrder::checkSpecialRequisitions(set<UnitType> *requisitions, Position basePosition) {
	for (std::set<Unit *>::const_iterator i = Broodwar->getUnitsInRadius(basePosition, max_distance).begin(); i != Broodwar->getUnitsInRadius(basePosition, max_distance).end(); i++) {
	}
}





void ProtossBuildOrder::checkMilitaryRequisitions(set<UnitType> *requisitions, Position basePosition) {
	int P_Gateway = 0;
	int P_Cybernetics_Core = 0;
	for (std::set<Unit *>::const_iterator i = Broodwar->getUnitsInRadius(basePosition, max_distance).begin(); i != Broodwar->getUnitsInRadius(basePosition, max_distance).end(); i++) {
		if((*i)->getType() == UnitTypes::Protoss_Gateway){
			P_Gateway ++;
		}
		if((*i)->getType() == UnitTypes::Protoss_Cybernetics_Core){
			P_Cybernetics_Core++;
		}
	}
	if(P_Gateway < Protoss_Gateway){
		requisitions->insert(UnitTypes::Protoss_Gateway);
	}
	if(P_Cybernetics_Core == 0 && P_Gateway >= 1){
		requisitions->insert(UnitTypes::Protoss_Cybernetics_Core);	
	}
}

void ProtossBuildOrder::checkResourceRequisitions(set<UnitType> *requisitions, Position basePosition) {
	if(Broodwar->self()->supplyUsed() - 5*Broodwar->elapsedTime()/180 < Broodwar->self()->supplyTotal()){
		requisitions->insert(UnitTypes::Protoss_Pylon);
	}
}

void ProtossBuildOrder::checkSpecialRequisitions(set<UnitType> *requisitions, Position basePosition) {

}
