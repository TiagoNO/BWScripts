#include "Expand.h"
#include <math.h>
using namespace BWAPI;

typedef struct InformationMinerals
{
	int PosiX;
	int PosiY;
	int Id;
	int NumBase;
}InfoMineral;

typedef struct CellMinerals
{
	InfoMineral Info;
	struct CellMinerals *prox;
}CellM;

typedef struct InformationBases
{
	int PosiX;
	int PosiY;
	int Id;
	int NumWorkers;
	int NumMineralField;
	int NumBase;
	bool BaseAlreadyBuilt;
	float WorkerPerMineral;
}InfoBase;

typedef struct CellBases
{
	InfoBase Info;
	CellM *Minerals;
	struct CellBases *prox;
}Cell;

int dist(int x1,int y1,int x2,int y2)
{
	int a = ((x1-x2)*(x1-x2));
	int b = ((y1-y2)*(y1-y2));
	return (int)sqrt((double)a+b);
}

CellM * InitializeMineralsList()
{
	CellM *NewMineral;
	NewMineral = (CellM *)malloc(sizeof(CellM));
	NewMineral->Info.Id = -1;
	NewMineral->Info.NumBase = -1;
	NewMineral->Info.PosiX = -1;
	NewMineral->Info.PosiY = -1;
	NewMineral->prox = NULL;
	return NewMineral;
}

void AddNewMineral(CellM *FinalCell,InfoMineral Info)
{
	CellM *NewMineral;
	NewMineral = (CellM *)malloc(sizeof(CellM));
	NewMineral->Info.Id = Info.Id;
	NewMineral->Info.PosiX = Info.PosiX;
	NewMineral->Info.PosiY = Info.PosiY;
	NewMineral->Info.NumBase = Info.NumBase;
	NewMineral->prox = NULL;
	FinalCell->prox = NewMineral;
}

Cell * InitializeBase()
{
	Cell *Bases;
	Bases = (Cell *)malloc(sizeof(Cell));
	Bases->Info.NumWorkers = 0;
	Bases->Info.NumMineralField = 0;
	Bases->Info.NumBase = -1;
	Bases->Info.WorkerPerMineral = 0;
	Bases->Info.BaseAlreadyBuilt = true;
	Bases->Minerals = InitializeMineralsList();
	Bases->Info.Id = -1;
	Bases->Minerals->Info.NumBase = Bases->Info.NumBase;
	Bases->Minerals->prox = NULL;
	Bases->prox = NULL;
	return Bases;
}

void AddNewBase(Cell *FinalCell,InfoBase Info)
{
	Cell *NewBase;
	NewBase = (Cell *)malloc(sizeof(Cell));
	NewBase->Info.Id = Info.Id;
	NewBase->Info.PosiX = Info.PosiX;
	NewBase->Info.PosiY = Info.PosiY;
	NewBase->Info.BaseAlreadyBuilt = Info.BaseAlreadyBuilt;
	NewBase->Info.NumMineralField = Info.NumMineralField;
	NewBase->Info.NumBase = Info.NumBase;
	NewBase->Info.NumWorkers = Info.NumWorkers;
	NewBase->Info.WorkerPerMineral = Info.WorkerPerMineral;
	FinalCell->prox = NewBase;
	NewBase->Minerals = InitializeMineralsList();
	NewBase->Minerals->Info.Id = Info.Id;
	NewBase->Minerals->Info.NumBase = NewBase->Info.NumBase;
	NewBase->Minerals->Info.PosiX = Info.PosiX;
	NewBase->Minerals->Info.PosiY = Info.PosiY;
	NewBase->prox = NULL;
}

int InAnyBase(Cell *Bases,Unit *Mineral)
{
	int foundBase = 0,foundMineral = 0;
	int NumberOfBases = 1;
	Cell *aux;
	aux = Bases;
	while(aux->prox != NULL)
	{
		if(dist(aux->Info.PosiX,aux->Info.PosiY,Mineral->getTilePosition().x(),Mineral->getTilePosition().y()) <= 20) // Checks if the mineral is close to any base already found
		{
			foundBase = 1;
			break;
		}
		NumberOfBases++;
		aux = aux->prox;
	}
	if(dist(aux->Info.PosiX,aux->Info.PosiY,Mineral->getTilePosition().x(),Mineral->getTilePosition().y()) <= 20) // Checks the last one of the list of bases
	{
		foundBase = 1;
	}
	if(foundBase) // if found the base where the mineral could be
	{
		CellM *aux2;
		aux2 = aux->Minerals;
		while(aux2->prox != NULL)  // checks if the mineral is in this base
		{
			if(aux2->Info.Id == Mineral->getID())
			{
				foundMineral = 1;
				break;
			}
			aux2 = aux2->prox;
		}
		if(aux2->Info.Id == Mineral->getID()) // checks the last cell of the list of minerals 
		{
			foundMineral = 1;
		}
		if(foundMineral)  // if it is already in the list, there´s no need to add it again
		{
			return 0;
		}
		else if(!foundMineral)  // if it is not in the list, then it needs to be add in
		{
			aux->Info.NumMineralField = aux->Info.NumMineralField++; 
			InformationMinerals Info;
			Info.Id = Mineral->getID();
			Info.PosiX = Mineral->getTilePosition().x();
			Info.PosiY = Mineral->getTilePosition().y();
			Info.NumBase = NumberOfBases;

			AddNewMineral(aux2,Info);
			return 1;
		}
	}
	else if(!foundBase)  // if it is not close to any base in the list, we found a new possible base! we need to add it to the list
	{
		InfoBase Info;
		Info.PosiX = Mineral->getTilePosition().x();
		Info.PosiY = Mineral->getTilePosition().y();
		Info.BaseAlreadyBuilt = false;
		Info.NumMineralField = 1;
		Info.NumWorkers = 0;
		Info.Id = Mineral->getID();
		Info.NumBase = NumberOfBases++;
		Info.WorkerPerMineral = 0;
		AddNewBase(aux,Info);
		return 2;
	}
}

Cell * RateOfWorkers(Cell *Bases,Unit *u)
{
	Cell *aux;
	aux = Bases;
	while(aux != NULL)
	{
		if(dist(aux->Info.PosiX,aux->Info.PosiY,u->getTilePosition().x(),u->getTilePosition().y()) < 20)
		{
			return aux;
		}
		aux = aux->prox;
	}
	return NULL;
}

TilePosition SearchBase(Cell *Bases)
{
	Cell *aux;
	aux = Bases;
	while(aux != NULL)
	{
		if(aux->Info.BaseAlreadyBuilt == false)
		{
			return TilePosition(aux->Info.PosiX,aux->Info.PosiY);
		}
		aux = aux->prox;
	}
	return TilePosition(0,0);
}

int CountNumWorkers(int x, int y)
{
	int count = 0;

	for(std::set<Unit*>::const_iterator u=Broodwar->self()->getUnits().begin();u!=Broodwar->self()->getUnits().end();u++)
	{
		if((*u)->getType().isWorker() && dist((*u)->getTilePosition().x(),(*u)->getTilePosition().y(),x,y) < 20)
		{
			Broodwar->sendText("(%i,%i)x(%i,%i)->{%i}",(*u)->getTilePosition().x(),(*u)->getTilePosition().y(),x,y,dist((*u)->getTilePosition().x(),(*u)->getTilePosition().y(),x,y));
			count++;
		}
	}
	Broodwar->sendText("%i workers",count);
	return count;
}

int CountNumMineralField(int x,int y)
{
	int count = 0;
    for(std::set<Unit*>::iterator u=Broodwar->getMinerals().begin();u!=Broodwar->getMinerals().end();u++)
    {
		if((*u)->getType().isMineralField() && dist((*u)->getTilePosition().x(),(*u)->getTilePosition().y(),x,y) < 20)
		{
			Broodwar->sendText("(%i,%i)x(%i,%i)->{%i}",(*u)->getTilePosition().x(),(*u)->getTilePosition().y(),x,y,dist((*u)->getTilePosition().x(),(*u)->getTilePosition().y(),x,y));
			count++;
		}
    }
	Broodwar->sendText("%i minerals",count);
	return count;
}

TilePosition NewBaseExpand(TilePosition Base,Unit *u)
{
	TilePosition aux;
	if(Base.x()*32 > (Broodwar->mapWidth()/2)*32)
	{
		if(Base.y()*32 > (Broodwar->mapHeight()/2)*32)
		{
			aux.x() = Base.x() - rand()%10;
			aux.y() = Base.y() - rand()%10;
			Broodwar->drawCircle(CoordinateType::Map,Base.x(),Base.y(),10,Colors::Red,false);
		}
		else if(Base.y()*32 <= Broodwar->mapHeight()*32)
		{
			aux.x() = Base.x() - rand()%10;
			aux.y() = Base.y() + rand()%10;
			Broodwar->drawCircle(CoordinateType::Map,Base.x(),Base.y(),10,Colors::Red,false);
		}
	}
	else if(Base.x()*32 <= (Broodwar->mapWidth()/2)*32)
	{
		if(Base.y()*32 > (Broodwar->mapHeight()/2)*32)
		{

			aux.x() = Base.x() + rand()%10;
			aux.y() = Base.y() - rand()%10;
			Broodwar->drawCircle(CoordinateType::Map,Base.x(),Base.y(),10,Colors::Red,false);
		}
		else if(Base.y()*32 <= Broodwar->mapHeight()*32)
		{
			aux.x() = Base.x() + rand()%10;
			aux.y() = Base.y() + rand()%10;
			Broodwar->drawCircle(CoordinateType::Map,Base.x(),Base.y(),10,Colors::Red,false);
		}
	}
	if(Broodwar->canBuildHere(u,aux,Broodwar->self()->getRace().getCenter()))
	{
		Broodwar->drawCircle(CoordinateType::Map,Base.x(),Base.y(),10,Colors::Red,false);
		return aux;
	}
	else
	{
		return TilePosition(0,0);
	}
}

void RemoveDestroyedBase(Cell *Bases,Unit *u)
{
	Cell *aux;
	Cell *removedCell;
	removedCell = Bases;
	if(removedCell->prox != NULL)
	{
		aux = removedCell->prox;
		while(aux != NULL)
		{
			if(aux->Info.Id == u->getID())
			{
				removedCell->prox = aux->prox;
				aux->prox = NULL;
				free(aux);
				break;
			}
			aux = aux->prox;
			removedCell = removedCell->prox;
		}
	}
	removedCell = NULL;
	free(removedCell);
}

void RemoveDestroyedMineral(Cell *Bases,Unit *u)
{
	Cell *searchingBase;
	bool foundBase = false;

	searchingBase = Bases;
	while(searchingBase != NULL)
	{
		if(dist(searchingBase->Info.PosiX,searchingBase->Info.PosiY,u->getPosition().x(),u->getPosition().y()) <= 20)
		{
			foundBase = true;
			break;
		}
		searchingBase = searchingBase->prox;
	}
	if(foundBase)
	{
		CellM *searchingMineral;
		CellM *removedMineral;
		removedMineral = searchingBase->Minerals;
		if(removedMineral->prox != NULL)
		{
			searchingMineral = removedMineral->prox;
			while(searchingMineral != NULL)
			{
				if(searchingMineral->Info.Id == u->getID())
				{
					removedMineral->prox = searchingMineral->prox;
					searchingMineral->prox = NULL;
					free(searchingMineral);
					break;
				}
				removedMineral = removedMineral->prox;
				searchingMineral = searchingMineral->prox;
			}
			removedMineral = NULL;
			free(removedMineral);
		}
	}
}

void UpdateBases(Cell *Bases,TilePosition WhereIsConstructing,int Id,bool Add,bool WhatChange)
{
	Cell *aux;
	aux = Bases;
	while(aux != NULL)
	{
		Broodwar->drawLine(CoordinateType::Map,WhereIsConstructing.x()*32,WhereIsConstructing.y()*32,aux->Info.PosiX*32,aux->Info.PosiY*32,Colors::Red);
		Broodwar->sendText("(%i,%i) Built: %i",aux->Info.PosiX,aux->Info.PosiY,aux->Info.BaseAlreadyBuilt);
		if(WhatChange)
		{
			if(dist(WhereIsConstructing.x(),WhereIsConstructing.y(),aux->Info.PosiX,aux->Info.PosiY) <= 20 && Add)
			{
				aux->Info.PosiX = WhereIsConstructing.x();
				aux->Info.PosiY = WhereIsConstructing.y();
				aux->Info.BaseAlreadyBuilt = true;
				aux->Info.NumWorkers = 1;
				break;
			}
			else if(dist(WhereIsConstructing.x(),WhereIsConstructing.y(),aux->Info.PosiX,aux->Info.PosiY) <= 20 && !Add)
			{
				aux->Info.BaseAlreadyBuilt = false;
				break;
			}
		}
		else if(!WhatChange)
		{
			if(dist(WhereIsConstructing.x(),WhereIsConstructing.y(),aux->Info.PosiX,aux->Info.PosiY) <= 20)
			{
				aux->Info.Id = Id;
			}
		}
		aux = aux->prox;
	}
	aux = NULL;
	free(aux);
}

float GetWorkersAvarege(Cell *Bases)
{
	Cell *aux;
	float Avarege = 0;
	float Minerals = 0;
	aux = Bases;
	while(aux != NULL)
	{
		Broodwar->sendText("%f (%f/%f) {%i-%i}",Avarege/Minerals,Avarege,Minerals,aux->Info.NumWorkers,aux->Info.NumMineralField);
		Avarege = Avarege + aux->Info.NumWorkers;
		Minerals = Minerals + aux->Info.NumMineralField;
		aux = aux->prox;
	}
	return float(Avarege/Minerals);
}

Cell * GetUnbalancedBase(Cell *Bases,float Avarege)
{
	Cell *aux;
	aux = Bases;
	while(aux != NULL)
	{
		if(aux->Info.WorkerPerMineral < Avarege)
		{
			return aux;
		}
		aux = aux->prox;
	}
	return NULL;
}

void BalanceBases(Cell *Bases,Unit *unit)
{
	float Avarege = GetWorkersAvarege(Bases);
	Cell *WhereTheWorkerLeaves,*WhereTheWorkerGoes;
	WhereTheWorkerLeaves = Bases;
	while(WhereTheWorkerLeaves != NULL)
	{
		WhereTheWorkerGoes = Bases;
		while(WhereTheWorkerGoes != NULL)
		{
			if(WhereTheWorkerGoes->Info.WorkerPerMineral < Avarege)
			{
				for(std::set<Unit *>::const_iterator Wor=Broodwar->getAllUnits().begin();Wor!=Broodwar->getAllUnits().end();Wor++)
				{
					if(dist(WhereTheWorkerLeaves->Info.PosiX,WhereTheWorkerLeaves->Info.PosiY,(*Wor)->getTilePosition().x(),(*Wor)->getTilePosition().y()) < 20)
					{
						if(WhereTheWorkerLeaves->Info.WorkerPerMineral > Avarege && WhereTheWorkerGoes->Info.WorkerPerMineral < Avarege)
						{
							if((*Wor)->getType().isWorker() && !(*Wor)->isConstructing())
							{
								Broodwar->drawCircle(CoordinateType::Map,(*Wor)->getPosition().x(),(*Wor)->getPosition().y(),10,Colors::Green,true);
								(*Wor)->move(unit->getPosition());
								WhereTheWorkerLeaves->Info.NumWorkers = WhereTheWorkerLeaves->Info.NumWorkers - 1;
								WhereTheWorkerLeaves->Info.WorkerPerMineral = float(float(WhereTheWorkerLeaves->Info.NumWorkers)/float(WhereTheWorkerLeaves->Info.NumMineralField));
								WhereTheWorkerGoes->Info.NumWorkers = WhereTheWorkerGoes->Info.NumWorkers + 1;
								WhereTheWorkerGoes->Info.WorkerPerMineral = float(float(WhereTheWorkerGoes->Info.NumWorkers)/float(WhereTheWorkerGoes->Info.NumMineralField));
								Broodwar->sendText("Movendo... {%i,%f}, {%i,%f}",WhereTheWorkerLeaves->Info.NumWorkers,WhereTheWorkerLeaves->Info.WorkerPerMineral,WhereTheWorkerGoes->Info.NumWorkers,WhereTheWorkerGoes->Info.WorkerPerMineral);
							}
						}
						else
						{
							break;
						}
					}
				}
			}
			WhereTheWorkerGoes = WhereTheWorkerGoes->prox;
		}
		WhereTheWorkerLeaves = WhereTheWorkerLeaves->prox;
	}
}

TilePosition findPylon(Position a)
{
	for(std::set<Unit *>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if((*i)->getType() == BWAPI::UnitTypes::Protoss_Pylon)
		{
			if(dist((*i)->getPosition().x(),(*i)->getPosition().y(),a.x(),a.y()) < 200)
			{
				return (*i)->getTilePosition();
			}
		}
	}
	return TilePosition(0,0);
}

void InitializeInMiddleGame(Cell *Bases,Unit *unit,int NumBases)
{
	if(Bases->Info.Id == -1)
	{
		Bases->Info.BaseAlreadyBuilt = true;
		Bases->Info.Id = unit->getID();
		Bases->Info.NumBase = NumBases;
		Bases->Info.NumMineralField = CountNumMineralField(unit->getTilePosition().x(),unit->getTilePosition().y());
		Bases->Info.NumWorkers = CountNumWorkers(unit->getTilePosition().x(),unit->getTilePosition().y());
		Bases->Info.PosiX = unit->getTilePosition().x();
		Bases->Info.PosiY = unit->getTilePosition().y();
		Bases->Info.WorkerPerMineral = (float) Bases->Info.NumWorkers/Bases->Info.NumMineralField;
		Bases->Minerals = InitializeMineralsList();
		Bases->prox = NULL;
	}
	else
	{
		Cell *aux;
		aux = Bases;
		while(aux->prox != NULL)
		{
			if(aux->Info.Id == unit->getID())
			{
				Broodwar->sendText("Já está adicionado a lista");
				break;
			}
			aux = aux->prox;
		}
		if(aux->Info.Id == unit->getID())
		{
			Broodwar->sendText("Já está adicionado a lista");
		}
		else
		{
			Cell *aux2;
			Broodwar->sendText("Não está na lista, mas está sendo adicionado agora");
			aux2 = (Cell *)malloc(sizeof(Cell));
			aux2->Info.BaseAlreadyBuilt = true;
			aux2->Info.Id = unit->getID();
			aux2->Info.NumBase = NumBases;
			aux2->Info.NumMineralField = CountNumMineralField(unit->getTilePosition().x(),unit->getTilePosition().y());
			aux2->Info.NumWorkers = CountNumWorkers(unit->getTilePosition().x(),unit->getTilePosition().y());
			aux2->Info.PosiX = unit->getTilePosition().x();
			aux2->Info.PosiY = unit->getTilePosition().y();
			aux2->Info.WorkerPerMineral = (float) aux2->Info.NumWorkers/aux2->Info.NumMineralField;
			aux2->Minerals = InitializeMineralsList();
			aux2->prox = NULL;
			aux->prox = aux2;
		}
		aux = NULL;
		free(aux);
	}
}

void PrintBases(Cell *Bases)
{
	Cell *aux = Bases;
	while(aux != NULL)
	{
		aux = aux->prox;
	}
}

void buildAssimilator(TilePosition gas)
{
	for(std::set<Unit *>::const_iterator i = Broodwar->self()->getUnits().begin(); i!=Broodwar->self()->getUnits().end(); i++)
	{
		if((*i)->getType().isWorker() && dist((*i)->getTilePosition().x(),(*i)->getTilePosition().y(),gas.x(),gas.y()) < 200)
		{
			(*i)->build(gas,UnitTypes::Protoss_Assimilator);
		}
	}
}

bool hasAnalysed;
bool analysisJustFinished;
BWTA::Region* baseHome; // BWTA info
BWTA::Region* enemyBase;

int NumBases; // Number of bases that the bot owns
Cell *Bases; // list of information of each base and mineral spots
int IdExplorer = -1; // Id of the unit that is responsible of walking in the map for information purpose
int Workers; // Number of workers that the bot owns
int UnitsBuildLimit; // the total units that can be created
int IdConstructor; // Id of the unit that is responsible of construct the new base
TilePosition NewBase; // Position where the new base will be
bool Built; // Found a valid position to build a new base next to an mineral field
bool Expanding; // The unit that will construct the new base is walking to the mineral field where it is going to be 
bool InExpansion; // The base is being constructed, to avoid construct new bases in that region
Cell *aux; // auxiliar pointer to get the initial information of the bases and its minerals
int Protector; // avoid some errors
int resetExpansion; // if there was an error in the expansion, we just reset it!
bool BuiltGat; // bool so that we know that our gateway is ready!
bool BuiltResearch; // bool so that we know that we can build Dragoons now!!
bool InGame; // bool so that we know that this behavior was called in the middle of the game
BWAPI::Position MoveTo; // position where the last minerals were found
int timeNewGat; // time since the last gat were built!

void Expand::onStart()
{
  InGame = false;
  BWTA::readMap();
  hasAnalysed=false;
  analysisJustFinished=false;

  show_bullets=false;
  show_visibility_data=false;

  if (Broodwar->isReplay())
  {
    for(std::set<Player*>::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
    {
      if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
      {
        Broodwar->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
      }
    }
  }
  else
  {
	if(Broodwar->self()->getRace() == Races::Zerg)
	{
		UnitsBuildLimit = 1;
	}
	else if(Broodwar->self()->getRace() == Races::Protoss)
	{
		UnitsBuildLimit = 9;
	}
	else if(Broodwar->self()->getRace() == Races::Terran)
	{
		UnitsBuildLimit = 10;
	}
    for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
    {
		if((*i)->getType() == UnitTypes::Protoss_Cybernetics_Core)
		{
			BuiltResearch = true;
		}
		if((*i)->getType() == UnitTypes::Terran_Supply_Depot)
		{
			UnitsBuildLimit = UnitsBuildLimit + 8;
		}
		else if((*i)->getType() == UnitTypes::Zerg_Overlord)
		{
			UnitsBuildLimit = UnitsBuildLimit + 8;
		}
		else if((*i)->getType() == UnitTypes::Protoss_Pylon)
		{
			UnitsBuildLimit = UnitsBuildLimit + 8;
		}
		if ((*i)->getType().isWorker())
		{
			Workers++;
			if(IdExplorer == -1) // gets the unit that will explore the map
			{
				IdExplorer = (*i)->getID();
			}
			if(IdConstructor == -1 && (*i)->getID() != IdExplorer) // gets the unit that will Expand when there is enought minerals
			{
				IdConstructor = (*i)->getID();
			}
		}
		if ((*i)->getType().isResourceDepot())
		{
		}
	}
  }
}

void Expand::onEnd(bool isWinner)
{
  if (isWinner)
  {
    //log win to file
  }
}

void Expand::onFrame()
{
	if (show_visibility_data)
	{
		drawVisibilityData();
	}
	if (show_bullets)
	{
		drawBullets();
	}
	if(Broodwar->getFrameCount()%4286 == 0)
	{
	}
	if(!InGame)
	{
		if(Bases == NULL)
		{
			  Bases = InitializeBase();
			  NumBases = 1;
			  Built = false;
			  Workers = 0;
			  IdConstructor = -1;
			  Expanding = false;
			  Protector = 0;
			  InGame = false;
			  MoveTo = Position(0,0);
			  BuiltResearch = false;
			  BuiltGat = false;
			  resetExpansion = 0;
		}
		if(Broodwar->self()->getRace() == Races::Zerg)
		{
			UnitsBuildLimit = 1;
		}
		else if(Broodwar->self()->getRace() == Races::Protoss)
		{
			UnitsBuildLimit = 9;
		}
		else if(Broodwar->self()->getRace() == Races::Terran)
		{
			UnitsBuildLimit = 10;
		}
		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
		{
			if((*i)->getType() == UnitTypes::Protoss_Cybernetics_Core)
			{
				BuiltResearch = true;
			}
			if((*i)->getType() == UnitTypes::Terran_Supply_Depot)
			{
				UnitsBuildLimit = UnitsBuildLimit + 8;
			}
			else if((*i)->getType() == UnitTypes::Zerg_Overlord)
			{
				UnitsBuildLimit = UnitsBuildLimit + 8;
			}
			else if((*i)->getType() == UnitTypes::Protoss_Pylon)
			{
				UnitsBuildLimit = UnitsBuildLimit + 8;
			}
			if ((*i)->getType().isWorker())
			{
				Workers++;
				if(IdExplorer == -1) // gets the unit that will explore the map
				{
					IdExplorer = (*i)->getID();
				}
				if(IdConstructor == -1 && (*i)->getID() != IdExplorer) // gets the unit that will Expand when there is enought minerals
				{
					IdConstructor = (*i)->getID();
				}
			}
			if ((*i)->getType().isResourceDepot())
			{
				InitializeInMiddleGame(Bases,(*i),NumBases);
				NumBases++;
			}
		}
		InGame = true;
	}
	PrintBases(Bases);
	Broodwar->sendText("%i %i",BuiltGat,BuiltResearch);
	drawStats();
	if(hasAnalysed && Broodwar->getFrameCount()%30==0)
	{
		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
		{
			if ((*i)->getType().isWorker())
			{
				std::set<BWTA::Chokepoint*> chokepoints= baseHome->getChokepoints();
				double min_length=10000;
				BWTA::Chokepoint* choke=NULL;
				for(std::set<BWTA::Chokepoint*>::iterator c=chokepoints.begin();c!=chokepoints.end();c++)
				{
					double length=(*c)->getWidth();
					if (length<min_length || choke==NULL)
					{
						min_length=length;
						choke=*c;
					}
				}
				(*i)->rightClick(choke->getCenter());
				break;
			}
		}
	}
	if(hasAnalysed)
	{
		drawTerrainData();
	}
	if(analysisJustFinished)
	{
		analysisJustFinished=false;
	}


	if(Broodwar->self()->minerals() >= Broodwar->self()->getRace().getCenter().mineralPrice() && !Built)
	{
		NewBase = SearchBase(Bases); // Search for a Base to Expand
		if(NewBase != TilePosition(0,0)) // if found a base
		{
			Built = true; // let it know that it was found
		}
	}

	for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if((*i)->getType().isWorker())
		{
			if(IdConstructor == -1 && !(*i)->isConstructing() && !(*i)->isCarryingGas() && !(*i)->isCarryingMinerals() && (*i)->getID() != IdExplorer)
			{
				IdConstructor = (*i)->getID(); // selects a new Constructor, in case there are no one
			}
			if(IdExplorer == -1 && !(*i)->isConstructing() && !(*i)->isCarryingGas() && !(*i)->isCarryingMinerals() && (*i)->getID() != IdConstructor)
			{
				IdExplorer = (*i)->getID(); // selects a new Constructor, in case there are no one
			}
			if(IdExplorer == (*i)->getID() && (*i)->isIdle())
			{
				if(MoveTo != Position(0,0))
				{
					(*i)->move(MoveTo);
					MoveTo = Position(0,0);
				}
				else
				{
					Position movimento = Position(rand()%(Broodwar->mapWidth()*32),rand()%(Broodwar->mapHeight()*32));
					(*i)->move(movimento);  // move the explorer to a new position
				}
			}
			if((*i)->isIdle())
			{
				Unit *closestMineral = NULL;
				for(std::set<Unit*>::iterator m=Broodwar->getMinerals().begin();m!=Broodwar->getMinerals().end();m++)
				{
					if (closestMineral == NULL || (*i)->getDistance(*m)<(*i)->getDistance(closestMineral)) 
						closestMineral=*m;
					if((*m)->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser && Broodwar->self()->minerals() >= UnitTypes::Resource_Vespene_Geyser.mineralPrice())
					{
						buildAssimilator((*m)->getTilePosition());
						Broodwar->sendText("oi");
					}
				}
				if(closestMineral != NULL && (*i)->getID() != IdExplorer && (*i)->getID() != IdConstructor)
				{
					(*i)->gather(closestMineral);  // send the workers to mine
				}
				if((*i)->getID() == IdConstructor && !Built)
				{
					(*i)->gather(closestMineral); // send the constructor to mine
				}
			}
			if(!(*i)->isCarryingMinerals() && !(*i)->isCarryingGas())
			{
				if(Broodwar->self()->allUnitCount(UnitTypes::AllUnits) >= UnitsBuildLimit - 3) // build resources depots
				{
					int x = Bases->Info.PosiX;
					int y = Bases->Info.PosiY;
					int sinalx = rand()%2;
					int sinaly = rand()%2;
					if(sinalx == 1)
					{
						x = x + Broodwar->self()->getRace().getSupplyProvider().tileWidth()  - (rand()%10);
					}
					else if(sinalx == 0)
					{
						x = x + Broodwar->self()->getRace().getSupplyProvider().tileWidth()  + (rand()%10);
					}
					if(sinaly == 1)
					{
						y = y + Broodwar->self()->getRace().getSupplyProvider().tileHeight() - (rand()%10); // gets a randon position until it is valid
					}
					else if(sinaly == 0)
					{
						y = y + Broodwar->self()->getRace().getSupplyProvider().tileHeight() + (rand()%10);
					}
					if(Broodwar->self()->getRace() == Races::Terran && Broodwar->self()->minerals() > UnitTypes::Terran_Supply_Depot.mineralPrice() + 50)
					{
						if(Broodwar->canBuildHere((*i),TilePosition(x,y),UnitTypes::Terran_Supply_Depot))
						{
							Broodwar->drawCircleMap(x*32,y*32,10,Colors::Red,false);
							if(!(*i)->build(TilePosition(x,y),UnitTypes::Terran_Supply_Depot))
							{
								
							}
							else // build an supply provider
							{
								UnitsBuildLimit = UnitsBuildLimit + 8;
							}
						}
					}
					else if(Broodwar->self()->getRace() == Races::Protoss && Broodwar->self()->minerals() > UnitTypes::Protoss_Pylon.mineralPrice() + 50)
					{
						if(Broodwar->canBuildHere((*i),TilePosition(x,y),UnitTypes::Protoss_Pylon))
						{
							Broodwar->drawCircleMap(x*32,y*32,10,Colors::Red,false);
							if(!(*i)->build(TilePosition(x,y),UnitTypes::Protoss_Pylon))
							{
								
							}
							else // build an supply provider
							{
								UnitsBuildLimit = UnitsBuildLimit + 8;
							}
						}
					}
				}
			}
			if((*i)->getID() == IdConstructor && !(*i)->isConstructing() && !InExpansion) // if the unit is the constructor
			{
				if((*i)->getID() == IdConstructor && Expanding && Built && Broodwar->self()->minerals() <= Broodwar->self()->getRace().getCenter().mineralPrice() - 150)
				{
					(*i)->stop();
					Built = false;
					Expanding = false;
					UpdateBases(Bases,NewBase,-1,false,true);
				}
				//Broodwar->sendText("Built: %i  Expanding: %i InExpansion: %i",Built,Expanding,InExpansion);
				Broodwar->drawCircle(CoordinateType::Map,(*i)->getPosition().x(),(*i)->getPosition().y(),10,Colors::Red,false);
				Broodwar->drawCircle(CoordinateType::Map,NewBase.x()*32,NewBase.y()*32,10,Colors::Red,true);
				if(Built && !Expanding)
				{
					Broodwar->drawCircle(CoordinateType::Map,NewBase.x()*32,NewBase.y()*32,10,Colors::Red,true);
					NewBase = NewBaseExpand(NewBase,(*i));
					if(NewBase != TilePosition(0,0))
					{
						(*i)->move(Position(NewBase.x()*32,NewBase.y()*32));
						Expanding = true;
					}
					if(NewBase == TilePosition(0,0))
					{
						Built = false;
					}
				}
				if((*i)->isIdle() && Expanding)
				{
					if((*i)->build(NewBase,Broodwar->self()->getRace().getCenter()))
					{
						Expanding = false;
						Built = false;
						InExpansion = false;
						IdConstructor = -1;
						UpdateBases(Bases,NewBase,-1,true,true);
						NumBases++;
					}
					else
					{
						Built = false;
						Expanding = false;
						UpdateBases(Bases,NewBase,-1,false,true);
					}
				}
			}
		}
		if((*i)->getType().isResourceDepot() && !(*i)->isBeingConstructed() && !(*i)->isMorphing() && (*i)->isCompleted()) // build new workers
		{
			Broodwar->drawCircle(CoordinateType::Map,(*i)->getPosition().x(),(*i)->getPosition().y(),10,Colors::Red,false);
			Broodwar->sendText("%i %i",Workers,UnitsBuildLimit);
			if(Workers < 60)
			{
				Cell *aux;
				aux = RateOfWorkers(Bases,(*i));
				if(aux != NULL) 
				{
					Broodwar->drawCircle(CoordinateType::Map,aux->Info.PosiX*32,aux->Info.PosiY*32,10,Colors::Red,false);
					if(aux->Info.WorkerPerMineral < 2 && (*i)->isIdle())
					{
						if(Broodwar->self()->minerals() >= Broodwar->self()->getRace().getWorker().mineralPrice())
						{
							if((*i)->train(Broodwar->self()->getRace().getWorker()))
							{
								aux->Info.NumWorkers++;
								aux->Info.WorkerPerMineral = float(float(aux->Info.NumWorkers)/float(aux->Info.NumMineralField));
								Broodwar->sendText("%f",aux->Info.WorkerPerMineral);
								Workers++;
								aux = NULL;
								free(aux);
							}
						}
					}
				}
				else
				{
					Broodwar->sendText("ERROR!!!");
				}
			}
			if(Broodwar->self()->allUnitCount(UnitTypes::AllUnits) >= UnitsBuildLimit - 3 && Broodwar->self()->getRace() == Races::Zerg)
			{
				if(Broodwar->self()->minerals() > UnitTypes::Zerg_Overlord.mineralPrice() + 50)
				{
					if(!(*i)->train(UnitTypes::Zerg_Overlord))
					{
						
					}
					else
					{
						UnitsBuildLimit = UnitsBuildLimit + 8;
					}
				}
			}
		}
		if(Expanding && (*i)->getType().isResourceDepot() && (*i)->isCompleted())
		{
			Broodwar->sendText("N = %i %i",NumBases,resetExpansion);
			resetExpansion++;
		}
		else
		{
			resetExpansion = 0;
		}
		if(resetExpansion == NumBases)
		{
			Built = false;
			Expanding = false;
			UpdateBases(Bases,NewBase,-1,false,true);
		}
		if(Broodwar->self()->minerals() >= BWAPI::UnitTypes::Protoss_Gateway && (*i)->getType().isWorker())
		{
			TilePosition Pylon = findPylon((*i)->getPosition());
			Broodwar->drawCircle(CoordinateType::Map,Pylon.x()*32,Pylon.y()*32,10,Colors::Red,false);
			if(Pylon != TilePosition(0,0))
			{
				Broodwar->sendText("Possui pylon e dinheiro!!");
				if(!BuiltGat)
				{
					Pylon = TilePosition(Pylon.x()+rand()%30,Pylon.y()+rand()%30);
					Broodwar->drawCircle(CoordinateType::Map,Pylon.x()*32,Pylon.y()*32,10,Colors::Red,false);
					if((*i)->build(Pylon,UnitTypes::Protoss_Gateway))
					{
						BuiltGat = true;
						timeNewGat = Broodwar->getFrameCount()/24;
					}
					else
					{
						BuiltGat = false;
					}
				}
				if(!BuiltResearch)
				{
					Pylon = TilePosition(Pylon.x()+rand()%30,Pylon.y()+rand()%30);
					Broodwar->drawCircle(CoordinateType::Map,Pylon.x(),Pylon.y(),10,Colors::Red,true);
					if((*i)->build(Pylon,UnitTypes::Protoss_Cybernetics_Core))
					{
						BuiltResearch = true;
					}
					else
					{
						BuiltResearch = false;
					}
				}
			}
		}
		if(Broodwar->getFrameCount()/24 - timeNewGat >= 150)
		{
			BuiltGat = false;
			Broodwar->sendText("Time to build another gate!");
		}
		if((*i)->getType() == BWAPI::UnitTypes::Protoss_Gateway && (*i)->isIdle())
		{
			if(Broodwar->self()->minerals() >= BWAPI::UnitTypes::Protoss_Zealot.mineralPrice())
			{
				(*i)->train(BWAPI::UnitTypes::Protoss_Zealot);
			}
			if(Broodwar->self()->minerals() >= BWAPI::UnitTypes::Protoss_Dragoon.mineralPrice() && Broodwar->self()->gas() >= BWAPI::UnitTypes::Protoss_Dragoon.gasPrice())
			{
				if(BuiltResearch)
				{
					(*i)->train(BWAPI::UnitTypes::Protoss_Dragoon);
				}
			}
		}
	}
}

void Expand::onSendText(std::string text)
{
  if (text=="/show bullets")
  {
    show_bullets = !show_bullets;
  } else if (text=="/show players")
  {
    showPlayers();
  } else if (text=="/show forces")
  {
    showForces();
  } else if (text=="/show visibility")
  {
    show_visibility_data=!show_visibility_data;
  } else if (text=="/analyze")
  {
    if (hasAnalysed == false)
    {
      Broodwar->printf("Analyzing map... this may take a minute");
//     CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BWTA_AnalyzeThread, NULL, 0, NULL);
    }
  } else
  {
    Broodwar->printf("You typed '%s'!",text.c_str());
    Broodwar->sendText("%s",text.c_str());
  }
}

void Expand::onReceiveText(BWAPI::Player* player, std::string text)
{
  Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void Expand::onPlayerLeft(BWAPI::Player* player)
{
  Broodwar->sendText("%s left the game.",player->getName().c_str());
}

void Expand::onNukeDetect(BWAPI::Position target)
{
  if (target!=Positions::Unknown)
    Broodwar->printf("Nuclear Launch Detected at (%d,%d)",target.x(),target.y());
  else
    Broodwar->printf("Nuclear Launch Detected");
}

void Expand::onUnitDiscover(BWAPI::Unit* unit)
{
  bool DontAdd = false;
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
  {
	if(unit->getType().isMineralField())
	{
		for(std::set<Unit*>::const_iterator enem=Broodwar->enemy()->getUnits().begin();enem!=Broodwar->enemy()->getUnits().end();enem++)
		{
			if(dist(unit->getTilePosition().x(),unit->getTilePosition().y(),(*enem)->getTilePosition().x(),(*enem)->getTilePosition().y()) <= 20)
			{
				DontAdd = true;
			}
		}
		if(unit->isBeingGathered())
			DontAdd = true;
		if(!DontAdd)
		{
			MoveTo = unit->getPosition();
			Broodwar->drawCircle(BWAPI::CoordinateType::Screen,unit->getPosition().x(),unit->getPosition().y(),100,Colors::Green,false);
			int ValueOfReturn;
			ValueOfReturn = InAnyBase(Bases,unit);
			if(ValueOfReturn == 0)
			{
				Broodwar->sendText("Already was add to the list of bases");
			}
			else if(ValueOfReturn == 1)
			{
				Broodwar->sendText("This mineral is not in this base, it has been added!");
			}
			else if(ValueOfReturn == 2)
			{
				Broodwar->sendText("New Base found! It has been added to the list of bases!");
			}

		}
	}
  }
}

void Expand::onUnitEvade(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] was last accessible at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void Expand::onUnitShow(BWAPI::Unit* unit)
{
	bool DontAdd = false;
	if(unit->getType().isMineralField())
	{
		for(std::set<Unit*>::const_iterator enem=Broodwar->enemy()->getUnits().begin();enem!=Broodwar->enemy()->getUnits().end();enem++)
		{
			if(dist(unit->getTilePosition().x(),unit->getTilePosition().y(),(*enem)->getTilePosition().x(),(*enem)->getTilePosition().y()) <= 20)
			{
				if((*enem)->getType() == Broodwar->enemy()->getRace().getCenter())
				{
					DontAdd = true;
				}
				if((*enem)->getType().isWorker())
				{
					if((*enem)->getBuildType() == Broodwar->enemy()->getRace().getCenter())
					{
						DontAdd = true;
					}
				}

			}
		}
		if(unit->isBeingGathered())
		{
			DontAdd = true;
		}
		if(DontAdd)
		{
			RemoveDestroyedBase(Bases,unit);
		}
	}
}

void Expand::onUnitHide(BWAPI::Unit* unit)
{
 // if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    //Broodwar->sendText("A %s [%x] was last seen at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void Expand::onUnitCreate(BWAPI::Unit* unit)
{
  if (Broodwar->getFrameCount()>1)
  {
    if (!Broodwar->isReplay())
	{
		if(unit->getType() == Broodwar->self()->getRace().getCenter())
		{
		//		BalancearBases(Bases);
		}
	}
    else
    {
      /*if we are in a replay, then we will print out the build order
      (just of the buildings, not the units).*/
      if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
      {
        int seconds=Broodwar->getFrameCount()/24;
        int minutes=seconds/60;
        seconds%=60;
        Broodwar->sendText("%.2d:%.2d: %s creates a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
      }
    }
  }
}

void Expand::onUnitDestroy(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
		Broodwar->sendText("A %s [%x] has been destroyed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	if(unit->getType().isMineralField())
	{
		RemoveDestroyedMineral(Bases,unit);
	}
	else if(unit->getType().isResourceDepot())
	{
		RemoveDestroyedBase(Bases,unit);
	}
	if(unit->getType().isWorker())
	{
		if(unit->getID() == IdExplorer)
		{
			IdExplorer = -1;
		}
		else if(unit->getID() == IdConstructor)
		{
			IdConstructor = -1;
			if(Built)
			{
				if(Expanding)
				{
					Expanding = false;
				}
			}
		}
		Workers--;
	}
}

void Expand::onUnitMorph(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay())
    Broodwar->sendText("A %s [%x] has been morphed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
  else
  {
    if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
    {
      int seconds=Broodwar->getFrameCount()/24;
      int minutes=seconds/60;
      seconds%=60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
    }
  }
}

void Expand::onUnitRenegade(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay())
    Broodwar->sendText("A %s [%x] is now owned by %s",unit->getType().getName().c_str(),unit,unit->getPlayer()->getName().c_str());
}

void Expand::onSaveGame(std::string gameName)
{
  Broodwar->printf("The game was saved to \"%s\".", gameName.c_str());
}

DWORD WINAPI BWTA_AnalyzeThread()
{
  BWTA::analyze();

  //self start location only available if the map has base locations
  if (BWTA::getStartLocation(BWAPI::Broodwar->self())!=NULL)
  {
    baseHome = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
  }
  //enemy start location only available if Complete Map Information is enabled.
  if (BWTA::getStartLocation(BWAPI::Broodwar->enemy())!=NULL)
  {
    enemyBase = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
  }
  hasAnalysed   = true;
  analysisJustFinished = true;
  return 0;
}

void Expand::drawStats()
{
  std::set<Unit*> myUnits = Broodwar->self()->getUnits();
  Broodwar->drawTextScreen(5,0,"I have %d units:",myUnits.size());
  std::map<UnitType, int> unitTypeCounts;
  for(std::set<Unit*>::iterator i=myUnits.begin();i!=myUnits.end();i++)
  {
    if (unitTypeCounts.find((*i)->getType())==unitTypeCounts.end())
    {
      unitTypeCounts.insert(std::make_pair((*i)->getType(),0));
    }
    unitTypeCounts.find((*i)->getType())->second++;
  }
  int line=1;
  for(std::map<UnitType,int>::iterator i=unitTypeCounts.begin();i!=unitTypeCounts.end();i++)
  {
    Broodwar->drawTextScreen(5,16*line,"- %d %ss",(*i).second, (*i).first.getName().c_str());
    line++;
  }
}

void Expand::drawBullets()
{
  std::set<Bullet*> bullets = Broodwar->getBullets();
  for(std::set<Bullet*>::iterator i=bullets.begin();i!=bullets.end();i++)
  {
    Position p=(*i)->getPosition();
    double velocityX = (*i)->getVelocityX();
    double velocityY = (*i)->getVelocityY();
    if ((*i)->getPlayer()==Broodwar->self())
    {
      Broodwar->drawLineMap(p.x(),p.y(),p.x()+(int)velocityX,p.y()+(int)velocityY,Colors::Green);
      Broodwar->drawTextMap(p.x(),p.y(),"\x07%s",(*i)->getType().getName().c_str());
    }
    else
    {
      Broodwar->drawLineMap(p.x(),p.y(),p.x()+(int)velocityX,p.y()+(int)velocityY,Colors::Red);
      Broodwar->drawTextMap(p.x(),p.y(),"\x06%s",(*i)->getType().getName().c_str());
    }
  }
}

void Expand::drawVisibilityData()
{
  for(int x=0;x<Broodwar->mapWidth();x++)
  {
    for(int y=0;y<Broodwar->mapHeight();y++)
    {
      if (Broodwar->isExplored(x,y))
      {
        if (Broodwar->isVisible(x,y))
          Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Green);
        else
          Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Blue);
      }
      else
        Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Red);
    }
  }
}

void Expand::drawTerrainData()
{
  //we will iterate through all the base locations, and draw their outlines.
  for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
  {
    TilePosition p=(*i)->getTilePosition();
    Position c=(*i)->getPosition();

    //draw outline of center location
    Broodwar->drawBox(CoordinateType::Map,p.x()*32,p.y()*32,p.x()*32+4*32,p.y()*32+3*32,Colors::Blue,false);

    //draw a circle at each mineral patch
    for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getStaticMinerals().begin();j!=(*i)->getStaticMinerals().end();j++)
    {
      Position q=(*j)->getInitialPosition();
      Broodwar->drawCircle(CoordinateType::Map,q.x(),q.y(),30,Colors::Cyan,false);
    }

    //draw the outlines of vespene geysers
    for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getGeysers().begin();j!=(*i)->getGeysers().end();j++)
    {
      TilePosition q=(*j)->getInitialTilePosition();
      Broodwar->drawBox(CoordinateType::Map,q.x()*32,q.y()*32,q.x()*32+4*32,q.y()*32+2*32,Colors::Orange,false);
    }

    //if this is an island expansion, draw a yellow circle around the base location
    if ((*i)->isIsland())
      Broodwar->drawCircle(CoordinateType::Map,c.x(),c.y(),80,Colors::Yellow,false);
  }

  //we will iterate through all the regions and draw the polygon outline of it in green.
  for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
  {
    BWTA::Polygon p=(*r)->getPolygon();
    for(int j=0;j<(int)p.size();j++)
    {
      Position point1=p[j];
      Position point2=p[(j+1) % p.size()];
      Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Green);
    }
  }

  //we will visualize the chokepoints with red lines
  for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
  {
    for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
    {
      Position point1=(*c)->getSides().first;
      Position point2=(*c)->getSides().second;
      Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Red);
    }
  }
}

void Expand::showPlayers()
{
  std::set<Player*> players=Broodwar->getPlayers();
  for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
  {
    Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
  }
}

void Expand::showForces()
{
  std::set<Force*> forces=Broodwar->getForces();
  for(std::set<Force*>::iterator i=forces.begin();i!=forces.end();i++)
  {
    std::set<Player*> players=(*i)->getPlayers();
    Broodwar->printf("Force %s has the following players:",(*i)->getName().c_str());
    for(std::set<Player*>::iterator j=players.begin();j!=players.end();j++)
    {
      Broodwar->printf("  - Player [%d]: %s",(*j)->getID(),(*j)->getName().c_str());
    }
  }
}

void Expand::onUnitComplete(BWAPI::Unit *unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] has been completed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
  if(unit->getType() == Broodwar->self()->getRace().getCenter() && Broodwar->getFrameCount() > 48)
	{
		Broodwar->drawCircle(CoordinateType::Map,unit->getPosition().x(),unit->getPosition().y(),10,Colors::Red,false);
		UpdateBases(Bases,NewBase,unit->getID(),false,false);
		BalanceBases(Bases,unit);
	}
}
