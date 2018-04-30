#include "Manual.h"
#include <math.h>
using namespace BWAPI;
#define BASE_DIST 400
#define MAX_INT 999999

typedef struct BaseCell{
	int numWorkers;
	int numMinerals;
	TilePosition baseTilePosition;
	Position basePosition;
	bool BaseAlreadyBuilt;
	struct BaseCell *prox;
}BaseCell;

typedef struct BaseManager{
	BaseCell *Bases;
	std::map<int, Position> Minerals;
}BaseManager;

typedef struct BuildCell{
	TilePosition BuildingPosition;
	UnitType BuildingType;
	bool Constructing;
	struct BuildCell *prox;
}BuildCell;

typedef struct{
	BuildCell *first;
	BuildCell *last;
}BuildController;

int dist(Position a,Position b)
{
	return int(sqrt((double)(a.x() - b.x())*(a.x() - b.x()) + ((a.y() - b.y())*(a.y() - b.y()))));
}

Unit* getWorker()
{
	for(std::set<Unit *>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->getAllUnits().end();i++)
	{
		if((*i)->getType().isWorker())
		{
			return (*i);
		}
	}
	Broodwar->sendText("There is no worker!");
	return NULL;
}

void InitializeList(BuildController BController)
{
	BController.first = NULL;
	BController.last = NULL;
}

bool AddBuildingInList(BuildController *BController,TilePosition WhereToBuild,UnitType BuildingToBeBuilt,int *MineralsSpent,int *GasSpent,bool HighPriority)
{
	if(BuildingToBeBuilt.mineralPrice() + (*MineralsSpent) > Broodwar->self()->minerals() && BuildingToBeBuilt.gasPrice() + (*GasSpent) > Broodwar->self()->gas()) // if we dont have enought minerals we can't build
	{
		Broodwar->sendText("We need %i more minerals to build %s,try again later...",(BuildingToBeBuilt.mineralPrice() + MineralsSpent) - Broodwar->self()->minerals(),BuildingToBeBuilt.getName().c_str());
		return false;
	}
	if(BController->first == NULL)
	{
		BController->first = (BuildCell *)malloc(sizeof(BuildCell));
		BController->first->BuildingPosition = WhereToBuild;
		BController->first->BuildingType = BuildingToBeBuilt;
		BController->first->Constructing = false;
		BController->first->prox = NULL;
		BController->last = BController->first;
		Broodwar->sendText("Added in the first");
		return true;
	}
	else
	{
		if(HighPriority)
		{
			BuildCell *aux = (BuildCell *)malloc(sizeof(BuildCell));
			aux->BuildingPosition = WhereToBuild;
			aux->BuildingType = BuildingToBeBuilt;
			aux->Constructing = false;
			aux->prox = BController->first;
			BController->first = aux;
			Broodwar->sendText("Added in the first in the list because it has a high priority");
			return true;
		}
		else
		{
			BController->last->prox = (BuildCell *)malloc(sizeof(BuildCell));
			BController->last->prox->BuildingPosition = WhereToBuild;
			BController->last->prox->BuildingType = BuildingToBeBuilt;
			BController->last->Constructing = false;
			BController->last->prox->prox = NULL;
			BController->last = BController->last->prox;
			Broodwar->sendText("Added in the last position in the list");
			return true;
		}
	}
}

bool BuildFirstInList(BuildController *BController,Unit *unit,int *MineralsSpent,int *GasSpent)
{
	if(BController->first == NULL)
	{
	//	Broodwar->sendText("BUILD ERROR: There is no building in the list");
		return false;
	}
	if(BController->first->BuildingType.mineralPrice() + (*MineralsSpent) > Broodwar->self()->minerals() && BController->first->BuildingType.gasPrice() + (*GasSpent) > Broodwar->self()->gas()) // if we dont have enought minerals we can't build
	{
	//	Broodwar->sendText("BUILD ERROR:We need %i more minerals to build %s,try again later...",(BController->first->BuildingType.mineralPrice() + MineralsSpent) - Broodwar->self()->minerals(),BController->first->BuildingType.getName().c_str());
		return false;
	}
	BuildCell *aux;
	aux = BController->first;
	while(aux != NULL)
	{
	//	Broodwar->sendText("%s is %i",aux->BuildingType.getName().c_str(),aux->Constructing);
		if(!aux->Constructing)
		{
			break;
		}
		aux = aux->prox;
	}
	if(aux == NULL)
	{
	//	Broodwar->sendText("BUILD ERROR:All buildings are beeing constructed!");
		return false;
	}
	else
	{
		if(!Broodwar->canBuildHere(unit,aux->BuildingPosition,aux->BuildingType,false))
		{
		//	Broodwar->sendText("The position (%i,%i) is invalid, we removed %s of the list",aux->BuildingPosition.x(),aux->BuildingPosition.y(),aux->BuildingType.getName().c_str());
			BuildCell *aux2;
			aux2 = BController->first;
			BController->first = BController->first->prox;
			aux2->prox = NULL;
			free(aux2);
			return false;
		}
		if(unit->build(aux->BuildingPosition,aux->BuildingType))
		{
			(*MineralsSpent) += aux->BuildingType.mineralPrice();
			(*GasSpent) += aux->BuildingType.gasPrice();
			aux->Constructing = true;
		//	Broodwar->sendText("%s is now beeing constructed!",aux->BuildingType.getName().c_str());
			return true;
		}
		else
		{
		//	Broodwar->sendText("ERROR: Building was not possible, the unit couldn't do it!");
			Broodwar->drawCircleMap(aux->BuildingPosition.x()*32,aux->BuildingPosition.y()*32,10,Colors::Blue,false);
		}
	}
	return false;
}

bool IsOnBuildingList(BuildController BController,UnitType BuildingType)
{
	BuildCell *aux;
	aux = BController.first;
	while(aux != NULL)
	{
		if(aux->BuildingType == BuildingType)
		{
			return true;
		}
		aux = aux->prox;
	}
	return false;
}

bool IsConstructingBuildingInList(BuildController BController,UnitType BuildingType)
{
	BuildCell *aux;
	aux = BController.first;
	while(aux != NULL)
	{
		if(aux->BuildingType == BuildingType && aux->Constructing)
		{
			return true;
		}
		aux = aux->prox;
	}
	return false;
}

void RemoveCompletedBuildingInList(BuildController *BController,UnitType building)
{
	if(BController->first->BuildingType == building && BController->first->Constructing)
	{
		BuildCell *aux;
		aux = BController->first;
		BController->first = BController->first->prox;
		aux->prox = NULL;
		free(aux);
	}
	else
	{
		BuildCell *aux;
		BuildCell *previous;
		aux = BController->first;
		while(aux != NULL)
		{
			if(aux->BuildingType == building && aux->Constructing)
			{
				previous->prox = aux->prox;
				aux->prox = NULL;
				free(aux);
				break;
			}
			previous = aux;
			aux = aux->prox;
		}
	}
}

void ImprimeLista(BuildController BController)
{
	BuildCell *aux;
	aux = BController.first;
	if(aux == NULL)
	{
		Broodwar->sendText("The list of buildings is empty!");
		return;
	}
	while(aux != NULL)
	{
		Broodwar->sendText("%s is in state %i and position (%i,%i)",aux->BuildingType.getName().c_str(),aux->Constructing,aux->BuildingPosition.x(),aux->BuildingPosition.y());
		Broodwar->drawCircleMap(aux->BuildingPosition.x()*32,aux->BuildingPosition.y()*32,10,Colors::Green,false);
		aux = aux->prox;
	}
}

void InitializeBaseManager(BaseManager BsManager)
{
	BsManager.Bases = NULL;
}

bool ThereIsNoEnemyNearby(Position spot)
{
	for(std::set<Unit *>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if((*i)->getType().isBuilding())
		{
			if(dist(spot,(*i)->getPosition()) < BASE_DIST)
			{
				return false;
			}
		}
	}
	return true;
}

int countWorkersInBase(Position base)
{
	int numWorkers = 0;
	for(std::set<Unit *>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if((*i)->getType().isWorker() && dist((*i)->getPosition(),base) < BASE_DIST)
		{
			numWorkers++;
		}
	}
	return numWorkers;
}

int countMineralsInBase(Position base)
{
	int numMinerals = 0;
	for(std::set<Unit *>::const_iterator i=Broodwar->getMinerals().begin();i!=Broodwar->getMinerals().end();i++)
	{
		if(dist((*i)->getPosition(),base) < BASE_DIST)
		{
			numMinerals++;
		}
	}
	return numMinerals;
}

int AddBaseInManager(BaseManager *BsManager,Unit *unit)
{
	bool Safe = false;
	bool Exist = false;
	if(ThereIsNoEnemyNearby(unit->getPosition()))
	{
		Broodwar->sendText("This base is neutral, so we can add to the list!");
		Safe = true;
	}
	BaseCell *base;
	BaseCell *previousBase;
	previousBase = NULL;
	base = BsManager->Bases;
	while(base != NULL)
	{
		if(dist(base->basePosition,unit->getPosition()) < BASE_DIST)
		{
			Exist = true;
			break;
		}
		Broodwar->sendText("(%i,%i) %i",base->basePosition.x(),base->basePosition.y(),dist(base->basePosition,unit->getPosition()));
		previousBase = base;
		base = base->prox;
	}
	if(Safe && Exist)
	{
		if(BsManager->Minerals.find(unit->getID()) == BsManager->Minerals.end()) // if this is a new mineral discovered in our base
		{
			base->numMinerals += 1;
			BsManager->Minerals.insert(std::pair<int,Position>(unit->getID(),unit->getPosition()));
			return 0;
		}
		return 1; // this location still safe and we already added to the list of possible bases
	}
	else if(Safe && !Exist)
	{
		if(BsManager->Bases == NULL)
		{
			BsManager->Bases = (BaseCell *)malloc(sizeof(BaseCell));
			if(unit->getType().isResourceDepot())
			{
				BsManager->Bases->BaseAlreadyBuilt = true;
				BsManager->Bases->numMinerals = countMineralsInBase(unit->getPosition());
				BsManager->Bases->numWorkers = countWorkersInBase(unit->getPosition());
			}
			else
			{
				BsManager->Bases->BaseAlreadyBuilt = false;
				BsManager->Bases->numMinerals = 1;
				BsManager->Bases->numWorkers = 0;
				BsManager->Minerals.insert(std::pair<int,Position>(unit->getID(),unit->getPosition()));
			}
			BsManager->Bases->basePosition = unit->getPosition();
			BsManager->Bases->baseTilePosition = unit->getTilePosition();
			BsManager->Bases->prox = NULL;
		}
		else
		{
			base = (BaseCell *)malloc(sizeof(BaseCell));
			if(unit->getType().isResourceDepot())
			{
				base->BaseAlreadyBuilt = true;
				base->numMinerals = countMineralsInBase(unit->getPosition());
				base->numWorkers = countWorkersInBase(unit->getPosition());
			}
			else
			{
				base->BaseAlreadyBuilt = false;
				base->numMinerals = 1;
				base->numWorkers = 0;
				BsManager->Minerals.insert(std::pair<int,Position>(unit->getID(),unit->getPosition()));
			}
			base->basePosition = unit->getPosition();
			base->baseTilePosition = unit->getTilePosition();
			base->prox = NULL;
			previousBase->prox = base;
		}
		return 2; // this location still safe and we just added to the list of possible bases
	}
	else if(!Safe && Exist)
	{
		previousBase->prox = base->prox;
		base->prox = NULL;
		free(base);
		return 3; // this is not a safe location anymore, so we have to remove it from our list of possible bases
	}
	else if(!Safe && !Exist)
	{
		return 4; // this is an enemy base, so we dont add it to our list
	}
}

void Imprime(BaseManager *BsManager)
{
	BaseCell *aux;
	aux = BsManager->Bases;
	while(aux != NULL)
	{
		Broodwar->sendText("(%i,%i) %i",aux->basePosition.x(),aux->basePosition.y(),aux->BaseAlreadyBuilt);
		Broodwar->drawCircleMap(aux->basePosition.x(),aux->basePosition.y(),10,Colors::Blue,false);
		aux = aux->prox;
	}

}

TilePosition GetValidBuildingLocation(TilePosition Base,UnitType Building,Unit *unit)
{
	TilePosition buildingTilePosition;
	buildingTilePosition = Base;
	buildingTilePosition = TilePosition(Base.x()+5,Base.y()+5);
	if(rand()%2 == 1)
	{
		if(rand()%2 == 1)
		{
			buildingTilePosition = TilePosition(buildingTilePosition.x() + rand()%6,buildingTilePosition.y() + rand()%6);
		}
		else
		{
			buildingTilePosition = TilePosition(buildingTilePosition.x() + rand()%6,buildingTilePosition.y() - rand()%6);
		}
	}
	else
	{
		if(rand()%2 == 1)
		{
			buildingTilePosition = TilePosition(buildingTilePosition.x() - rand()%6,buildingTilePosition.y() + rand()%6);
		}
		else
		{
			buildingTilePosition = TilePosition(buildingTilePosition.x() - rand()%6,buildingTilePosition.y() - rand()%6);
		}
	}
	
	if(Broodwar->canBuildHere(unit,buildingTilePosition,Building,false))
	{
		return buildingTilePosition;
	}
	return TilePosition(0,0);
}

TilePosition GetPositionPylon(Unit *unit)
{
	Position minPosi = Position(MAX_INT,MAX_INT);
	TilePosition minTilePosi = TilePosition(MAX_INT,MAX_INT);
	for(std::set<Unit *>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if((*i)->getType() == UnitTypes::Protoss_Pylon)
		{
			if(dist(minPosi,unit->getPosition()) < dist((*i)->getPosition(),unit->getPosition()))
			{
				minTilePosi = (*i)->getTilePosition();
				minPosi = (*i)->getPosition();
			}
		}
	}
	return minTilePosi;
}

bool analyzed;
bool analysis_just_finished;
BWTA::Region* home;
BWTA::Region* enemy_base;
// unit build controller
int MineralsSpent; // is the amout of mineral spent in that frame
int GasSpent;	   // is the amout of gas spent in that frame
BuildController BController; // it's the list of buildings that we want to construct
int LastBuildingInList;  // it's the "pointer" to the last position in the array of buildings to be constructed
int FirstBuildingInList; // it's the "pointer" to the first position in the array of builfings to be constructed
int TimeToBuildFirtBuilding; // it's the time that a new building will be made
int TimeToBuildSecondBuilding; // it's the time that a new secondary building will be made
// unit build controller

//Base Manager
BaseManager BsManager;
//Base Manager



void ExampleAIModule::onStart()
{
	Broodwar->enableFlag(Flag::UserInput);
	BWTA::readMap();
	analyzed=false;
	analysis_just_finished=false;

	show_bullets=false;
	show_visibility_data=false;


	TimeToBuildFirtBuilding = 2160;
	TimeToBuildSecondBuilding = 2160;

	InitializeList(BController); // initialize the list of buildings
	InitializeBaseManager(BsManager);
	for(std::set<Unit *>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if((*i)->getType().isResourceDepot())
		{
			AddBaseInManager(&BsManager,(*i));
		}
	}
}

void ExampleAIModule::onEnd(bool isWinner)
{
  if (isWinner)
  {
    //log win to file
  }
}

void ExampleAIModule::onFrame()
{
	//Broodwar->sendText("Minerals Spent: %i Gas Spent: %i",MineralsSpent,GasSpent);
	if(show_visibility_data)
		drawVisibilityData();

	if (show_bullets)
		drawBullets();

	if (analyzed)
		drawTerrainData();

	if (analysis_just_finished)
	{
		analysis_just_finished=false;
	}

	//	Broodwar->sendText("frames: %i frameScale: %i TimeToBuildFirtsBuilding: %i",Broodwar->getFrameCount(),Broodwar->getFrameCount()%2160,TimeToBuildFirtBuilding);
}

void ExampleAIModule::onSendText(std::string text)
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
    if (analyzed == false)
    {
      Broodwar->printf("Analyzing map... this may take a minute");
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
    }
  } else
  {
    Broodwar->printf("You typed '%s'!",text.c_str());
    Broodwar->sendText("%s",text.c_str());
	if(text == "checkBuildList")
	{
		ImprimeLista(BController);
	}
	else if(text == "checkMGSpent")
	{
		Broodwar->sendText("%i %i",MineralsSpent,GasSpent);
	}
	else if(text == "checkBasesList")
	{
		Imprime(&BsManager);
	}
	else if(text == "frameInfo")
	{
		Broodwar->sendText("frames: %i frameScale: %i TimeToBuildFirtsBuilding: %i",Broodwar->getFrameCount(),Broodwar->getFrameCount()%2160,TimeToBuildFirtBuilding);
	}
  }
}

void ExampleAIModule::onReceiveText(BWAPI::Player* player, std::string text)
{
  Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player* player)
{
  Broodwar->sendText("%s left the game.",player->getName().c_str());
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{
  if (target!=Positions::Unknown)
    Broodwar->printf("Nuclear Launch Detected at (%d,%d)",target.x(),target.y());
  else
    Broodwar->printf("Nuclear Launch Detected");
}

void ExampleAIModule::onUnitDiscover(BWAPI::Unit* unit)
{
	if(!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
	{
		if(unit->getType().isMineralField())
		{
			int returnValue = AddBaseInManager(&BsManager,unit);
			if(returnValue == 0)
			{
				Broodwar->sendText("New Mineral was found in an existing base!");
			}
			else if(returnValue == 1)
			{
				Broodwar->sendText("Found an existing base!");
			}
			else if(returnValue == 2)
			{
				Broodwar->sendText("A new base found and it was added to the list!");
			}
			else if(returnValue == 3)
			{
				Broodwar->sendText("This base was removed because it was not safe anymore!");
			}
			else if(returnValue == 4)
			{
				Broodwar->sendText("Found an enemy base!");
			}		
		}
	}
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] was last accessible at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void ExampleAIModule::onUnitShow(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] has been spotted at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void ExampleAIModule::onUnitHide(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] was last seen at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit* unit)
{
	if (Broodwar->getFrameCount()>1)
	{
		Broodwar->sendText("1 => %i - %i, %i - %i",MineralsSpent,unit->getType().mineralPrice(),GasSpent,unit->getType().gasPrice());
		MineralsSpent -= unit->getType().mineralPrice();
		GasSpent -= unit->getType().gasPrice();
		Broodwar->sendText("1 => %i, %i",MineralsSpent,GasSpent);
	}
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] has been destroyed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit* unit)
{
	if(!Broodwar->isReplay())
	{
	}
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay())
    Broodwar->sendText("A %s [%x] is now owned by %s",unit->getType().getName().c_str(),unit,unit->getPlayer()->getName().c_str());
}

void ExampleAIModule::onSaveGame(std::string gameName)
{
  Broodwar->printf("The game was saved to \"%s\".", gameName.c_str());
}

DWORD WINAPI AnalyzeThread()
{
  BWTA::analyze();

  //self start location only available if the map has base locations
  if (BWTA::getStartLocation(BWAPI::Broodwar->self())!=NULL)
  {
    home       = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
  }
  //enemy start location only available if Complete Map Information is enabled.
  if (BWTA::getStartLocation(BWAPI::Broodwar->enemy())!=NULL)
  {
    enemy_base = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
  }
  analyzed   = true;
  analysis_just_finished = true;
  return 0;
}

void ExampleAIModule::drawStats()
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

void ExampleAIModule::drawBullets()
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

void ExampleAIModule::drawVisibilityData()
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

void ExampleAIModule::drawTerrainData()
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

void ExampleAIModule::showPlayers()
{
  std::set<Player*> players=Broodwar->getPlayers();
  for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
  {
    Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
  }
}

void ExampleAIModule::showForces()
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

void ExampleAIModule::onUnitComplete(BWAPI::Unit *unit)
{
	if(!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
	{
		Broodwar->sendText("unit completed");
		if(unit->getType().isBuilding())
		{
			RemoveCompletedBuildingInList(&BController,unit->getType());
		}
	}
}
