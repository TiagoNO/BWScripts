#include "Explore.h"
#include <math.h>
using namespace BWAPI;

#define pylo 0
#define cyber 1
#define gate 2

typedef struct BuildCell{
	TilePosition BuildTilePosition;
	UnitType building;
	struct BuildCell *prox;
}BuildCell;

BuildCell *InitializeBuildManager()
{
	BuildCell *BManager;
	BManager = (BuildCell *)malloc(sizeof(BuildCell));
	BManager->building = UnitTypes::None;
	BManager->BuildTilePosition = TilePosition(0,0);
	BManager->prox = NULL;
	return BManager;
}

void AddBuildingList(BuildCell *BManager,TilePosition buildingTilePosition,UnitType building,bool priority)
{
	BuildCell *aux = BManager;
	BuildCell *NewCell = (BuildCell *)malloc(sizeof(BuildCell));
	NewCell->building = building;
	NewCell->BuildTilePosition = buildingTilePosition;
	NewCell->prox = NULL;
	if(priority)
	{
		NewCell->prox = BManager->prox;
		BManager->prox = NewCell;
	}
	else
	{
		while(aux->prox != NULL)
		{
			aux = aux->prox;
		}
		aux->prox = NewCell;
	}
}

bool CheckUnitInBuildList(BuildCell *BManager,UnitType building)
{
	BuildCell *aux;
	aux = BManager;
	while(aux != NULL)
	{
		if(aux->building == building)
		{
			return true;
		}
		aux = aux->prox;
	}
	return false;
}

void PrintBuildList(BuildCell *BManager)
{
	int i = 1;
	BuildCell *aux;
	aux = BManager->prox;
	while(aux != NULL)
	{
		Broodwar->sendText("%i- %s {%i,%i}",i,aux->building.getName().c_str(),aux->BuildTilePosition.x(),aux->BuildTilePosition.y());
		i++;
		aux = aux->prox;
	}

}

bool BuildFirstList(BuildCell *BManager,Unit *u,UnitType *LastBuilt,int *LastBuildingPrice,bool *Buildings)
{
	if(BManager->prox != NULL && BManager->prox->building != UnitTypes::None)
	{
		Broodwar->sendText("is not empty");
	 	if(Broodwar->self()->minerals() - (*LastBuildingPrice) >= BManager->prox->building.mineralPrice())
		{
			Broodwar->sendText("there is mineral to build");
			if(Broodwar->canBuildHere(u,BManager->prox->BuildTilePosition,BManager->prox->building,true))
			{
				Broodwar->sendText("we can build there");
				if(u->build(BManager->prox->BuildTilePosition,BManager->prox->building))
				{
					Broodwar->sendText("the unit went to build");
					(*LastBuildingPrice) += BManager->prox->building.mineralPrice();
					(*LastBuilt) = BManager->prox->building;

					u->build(BManager->prox->BuildTilePosition,BManager->prox->building);
					
					Broodwar->sendText("The Building %s is now beeing constructed!",BManager->prox->building.getName().c_str());
					BManager->prox = BManager->prox->prox;
					return true;
				}
			}
			else
			{
				BuildCell *aux = BManager->prox;
				BManager->prox = BManager->prox->prox;
				aux->prox = NULL;
				if(aux->building == UnitTypes::Protoss_Pylon)
				{
					Buildings[pylo] = false;
				}
				else if(aux->building == UnitTypes::Protoss_Cybernetics_Core)
				{
					Buildings[cyber] = false;			
				}
				else if(aux->building == UnitTypes::Protoss_Gateway)
				{
					Buildings[gate] = false;
				}
				free(aux);
			}
		}
	}
	return false;
}

int distan(int x1,int y1,int x2,int y2)
{
	return (int)sqrt((double)(((x1 - x2)*(x1-x2)) + ((y1-y2)*(y1-y2))));
}

bool isVisited(Position place,std::set<Position> PositionsVisited)
{
	for(std::set<Position>::const_iterator iter=PositionsVisited.begin();iter!=PositionsVisited.end();iter++)
	{
		if(distan(place.x(),place.y(),(*iter).x(),(*iter).y()) < 300)
		{
			return true;
		}
	}
	return false;
}

TilePosition GetConstructionPosition(TilePosition buildP,int n)
{
	int x = buildP.x();
	int y = buildP.y();
	int sinalx = rand()%2;
	int sinaly = rand()%2;
	if(sinalx == 1)
	{
		x = buildP.x() - (rand()%n);
	}
	else if(sinalx == 0)
	{
		x = buildP.x()  + (rand()%n);
	}
	if(sinaly == 1)
	{
		y = buildP.y() - (rand()%n); // gets a randon position until it is valid
	}
	else if(sinaly == 0)
	{
		y = buildP.y() + (rand()%n);
	}
	else
	{
		return TilePosition(0,0);
	}
	return TilePosition(x,y);
}

TilePosition findPylon(TilePosition a)
{
	TilePosition b = TilePosition(99999,99999);
	for(std::set<Unit *>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if((*i)->getType() == BWAPI::UnitTypes::Protoss_Pylon)
		{
			Broodwar->drawCircle(CoordinateType::Map,b.x()*32,b.y()*32,10,Colors::Red,false);
			return (*i)->getTilePosition();
		}
	}
	return TilePosition(0,0);
}

bool CheckIfTheGroupisReunited(Unit *leader)
{
	int result = 1;
	Broodwar->drawCircle(CoordinateType::Map,leader->getPosition().x(),leader->getPosition().y(),150,Colors::Grey,false);
	for(std::set<Unit *>::const_iterator i=Broodwar->getUnitsInRadius(leader->getPosition(),150).begin();i!=Broodwar->getUnitsInRadius(leader->getPosition(),150).end();i++)
	{
		if(!(*i)->getType().isWorker() && !(*i)->getType().isBuilding())
		{
			result++;
		}
	}
	Broodwar->sendText("%i",result);
	if(result >= 6)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ReuniteExplorerGroup(Unit *leader,std::map<int,int> groups,int *numGroups)
{
	int k = 0;
	for(std::set<Unit *>::const_iterator i = Broodwar->self()->getUnits().begin(); i!= Broodwar->self()->getUnits().end();i++)
	{
		if(!(*i)->getType().isWorker() && !(*i)->getType().isBuilding() && !(*i)->isFollowing())
		{
			(*i)->follow(leader);
			k++;
		}
		if(k >= 5)
		{
			break;
		}
	}
	groups.insert(std::pair<int,int>((*numGroups),leader->getID()));
}

void MoveGroupOfExplorers(std::set<Position> PositionsVisited,std::map<int,int> groups,int numGroups)
{
	Position movimento = Position(rand()%(Broodwar->mapWidth()*32),rand()%(Broodwar->mapHeight()*32));
	Unit *leader;
	if(!isVisited(movimento,PositionsVisited))
	{
		for(int i = 0; i < numGroups;i++)
		{
			leader = Broodwar->getUnit(groups.find(i)->second);
			leader->move(movimento);
		}
	}
}

void GatherMinerals(Unit *worker)
{
	Unit *closestMineral = NULL;
	for(std::set<Unit*>::const_iterator m=Broodwar->getMinerals().begin();m!=Broodwar->getMinerals().end();m++)
	{
		if (closestMineral == NULL || worker->getDistance(*m)<worker->getDistance(closestMineral)) 
		{
			closestMineral=*m;
		}
	}
	if(closestMineral != NULL)
	{
		worker->gather(closestMineral);  // send the workers to mine
	}
}

TilePosition GetBuildPosition(TilePosition buildTilePosition,UnitType building,int n)
{
	TilePosition aux;
	if(buildTilePosition.x() > (Broodwar->mapWidth()/2))
	{
		if(buildTilePosition.y() > (Broodwar->mapHeight()/2))
		{
			aux.x() = buildTilePosition.x() - rand()%n;
			aux.y() = buildTilePosition.y() - rand()%n;
			Broodwar->drawCircle(CoordinateType::Map,aux.x()*32,aux.y()*32,10,Colors::Red,false);
		}
		else if(buildTilePosition.y() <= Broodwar->mapHeight())
		{
			aux.x() = buildTilePosition.x() - rand()%n;
			aux.y() = buildTilePosition.y() + rand()%n;
			Broodwar->drawCircle(CoordinateType::Map,aux.x()*32,aux.y()*32,10,Colors::Red,false);
		}
	}
	else if(buildTilePosition.x() <= (Broodwar->mapWidth()/2))
	{
		if(buildTilePosition.y() > (Broodwar->mapHeight()/2))
		{

			aux.x() = buildTilePosition.x() + rand()%n;
			aux.y() = buildTilePosition.y() - rand()%n;
			Broodwar->drawCircle(CoordinateType::Map,aux.x()*32,aux.y()*32,10,Colors::Red,false);
		}
		else if(buildTilePosition.y() <= Broodwar->mapHeight())
		{
			aux.x() = buildTilePosition.x() + rand()%n;
			aux.y() = buildTilePosition.y() + rand()%n;
			Broodwar->drawCircle(CoordinateType::Map,aux.x()*32,aux.y()*32,10,Colors::Red,false);
		}
	}
	return aux;
}

bool CheckTilePosition(TilePosition WhereToBuildPylon,UnitType building)
{
	for(std::set<Unit *>::const_iterator i = Broodwar->self()->getUnits().begin(); i!= Broodwar->self()->getUnits().end();i++)
	{
		if((*i)->getType().isWorker())
		{
			if(Broodwar->canBuildHere((*i),WhereToBuildPylon,building))
			{
				return true;
			}
		}
	}
	return false;
}

Unit *GetWorker()
{
	for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if((*i)->getType().isWorker())
		{
			return (*i);
		}
	}
}

bool analyzed;
bool analysis_just_finished;
BWTA::Region* home;
BWTA::Region* enemy_base;
int reset;
std::set<Position> PositionsVisited;
std::set<Position>::const_iterator iter;
bool NotVisited;
bool InGame2;
int LastBuildingPrice;
BuildCell *BManager;
int timeNewGat2;
UnitType LastBuilt;
bool Buildings[3];
std::map<int,int> groups;
int NumGroups = 0;

void Explore::onStart()
{
  // Enable some cheat flags
  Broodwar->enableFlag(Flag::UserInput);
  // Uncomment to enable complete map information
  //Broodwar->enableFlag(Flag::CompleteMapInformation);
  Broodwar->setLocalSpeed(1);
  //read map information into BWTA so terrain analysis can be done in another thread
  BWTA::readMap();
  analyzed=false;
  analysis_just_finished=false;

  show_bullets=false;
  show_visibility_data=false;

  InGame2 = false;
  NotVisited = true;
  reset = 0;

}

void Explore::onEnd(bool isWinner)
{
  if (isWinner)
  {
    //log win to file
  }
}

void Explore::onFrame()
{
	if(!InGame2) // if its the first frame in the game
	{
		LastBuildingPrice = 0;
		BManager = InitializeBuildManager();
		timeNewGat2 = 0;
		LastBuilt = UnitTypes::None;
		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
		{
			if((*i)->getType() == UnitTypes::Protoss_Cybernetics_Core)
			{
				Buildings[cyber] = true;
			}
		}
		InGame2 = true;
	}

  if (show_visibility_data)
    drawVisibilityData();

  if (show_bullets)
    drawBullets();

  if (Broodwar->isReplay())
    return;

  drawStats();
  if (analyzed && Broodwar->getFrameCount()%30==0)
  {
    for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
    {
      if ((*i)->getType().isWorker())
      {
        std::set<BWTA::Chokepoint*> chokepoints= home->getChokepoints();
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
  if (analyzed)
    drawTerrainData();

  if (analysis_just_finished)
  {
    analysis_just_finished=false;
  }
	reset = 0;
	PrintBuildList(BManager);
	for(std::set<Unit*>::const_iterator i = Broodwar->self()->getUnits().begin(); i != Broodwar->self()->getUnits().end(); i++) 
	{
		if(LastBuilt == (*i)->getType() && (*i)->isBeingConstructed()) // reset the total of the current minerals in use
		{
			Broodwar->sendText("The building is beeing constructed!");
			LastBuilt = UnitTypes::None;
			LastBuildingPrice = 0;
		}
		if (!(*i)->getType().isWorker() && (*i)->isIdle() && (*i)->canIssueCommand(UnitCommand::move((*i),Position(0,0),false)) && (*i)->isCompleted()) 
		{
			Position movimento = Position(rand()%(Broodwar->mapWidth()*32),rand()%(Broodwar->mapHeight()*32));
			if(!isVisited(movimento,PositionsVisited))
			{
				(*i)->move(movimento);
			}
			else
			{
				reset++;
			}
		}
		if((*i)->getType().isWorker())
		{
			if((*i)->isIdle())
			{
				GatherMinerals((*i)); // if the worker is doing nothing, go mine!
			}
			if(!(*i)->isCarryingMinerals() && !(*i)->isCarryingGas())
			{
				if(Broodwar->self()->minerals() >= BWAPI::UnitTypes::Protoss_Gateway.mineralPrice())
				{
					TilePosition Pylon = findPylon((*i)->getTilePosition()); // gets the closest pylon
					Broodwar->drawCircle(CoordinateType::Map,Pylon.x()*32,Pylon.y()*32,10,Colors::Red,false);
					if(Pylon != TilePosition(0,0)) // if there is a pylon
					{
						if(!Buildings[gate] && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Gateway) < 5) // if its time to build a gateway
						{
							Broodwar->sendText("i´ll try to build a gateway");
							Pylon = GetConstructionPosition(Pylon,15);
							Broodwar->drawCircle(CoordinateType::Map,Pylon.x()*32,Pylon.y()*32,10,Colors::Red,false);
							if(Broodwar->canBuildHere((*i),Pylon,UnitTypes::Protoss_Gateway,true)) //if we can build the gateway there
							{
								Broodwar->sendText("Gateway could be built");
								Buildings[gate] = true;
								timeNewGat2 = Broodwar->getFrameCount()/24; // set the timer
								AddBuildingList(BManager,Pylon,UnitTypes::Protoss_Gateway,false); // add the gateway to the build list
							}
							else
							{
								Broodwar->sendText("Gateway couldn´t be built");
								Buildings[gate] = false; // if it was not possible to build there, we just try it again next frame
							}
						}
						else if(!Buildings[cyber]) // if there is no cybernetics core, we try to build one
						{
							Broodwar->sendText("i´ll try to build a cybernetics core");
							Pylon = GetConstructionPosition(Pylon,15);
							Broodwar->drawCircle(CoordinateType::Map,Pylon.x(),Pylon.y(),10,Colors::Red,true);
							if(Broodwar->canBuildHere((*i),Pylon,UnitTypes::Protoss_Cybernetics_Core,true)) //if we can build there
							{
								Broodwar->sendText("cybernetics core could be built");
								Buildings[cyber] = true;  // we set true so that we know that we now can build Dragoons!
								AddBuildingList(BManager,Pylon,UnitTypes::Protoss_Cybernetics_Core,false); // add to the build list
							}
							else
							{
								Broodwar->sendText("cybernetics core couldn´t be built");
								Buildings[cyber] = false; // if fail, we try again next frame
							}
						}
					}
				}
				if(!BuildFirstList(BManager,(*i),&LastBuilt,&LastBuildingPrice,Buildings)) // if it wasn´t possible to build
				{

				}
			}
		}
		if((*i)->getType().isResourceDepot())
		{
			if(Broodwar->self()->supplyUsed() + 4  >= Broodwar->self()->supplyTotal() && !Buildings[pylo])
			{ // if is time to build a pylon
				Broodwar->sendText("Trying to build pylon");
				TilePosition WhereToBuildPylon = GetBuildPosition((*i)->getTilePosition(),UnitTypes::Protoss_Pylon,15);
				Broodwar->drawCircle(CoordinateType::Map,WhereToBuildPylon.x()*32,WhereToBuildPylon.y()*32,10,Colors::Green,false);
				if(CheckTilePosition(WhereToBuildPylon,UnitTypes::Protoss_Pylon))
				{
					Buildings[pylo] = true;
					Broodwar->sendText("pylon can be build!");
					AddBuildingList(BManager,WhereToBuildPylon,UnitTypes::Protoss_Pylon,true);
				}
				else
				{
					Broodwar->sendText("pylon can´t be build!");
					Buildings[pylo] = false;
				}
			}
			if((*i)->isIdle() && Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + LastBuilt.mineralPrice())
			{
				(*i)->train(UnitTypes::Protoss_Probe); // if we can train new workers, why not?
			}
		}
		if((*i)->getType() == BWAPI::UnitTypes::Protoss_Gateway && (*i)->isIdle())
		{
			if(Broodwar->self()->minerals() >= BWAPI::UnitTypes::Protoss_Dragoon.mineralPrice() + LastBuilt.mineralPrice() && Broodwar->self()->gas() >= BWAPI::UnitTypes::Protoss_Dragoon.gasPrice())
			{
				if(Buildings[cyber])
				{
					if(rand()%2 == 1)
					{
						(*i)->train(BWAPI::UnitTypes::Protoss_Dragoon);
					}
				}
			}
			if(Broodwar->self()->minerals() >= BWAPI::UnitTypes::Protoss_Zealot.mineralPrice() + LastBuilt.mineralPrice())
			{
				(*i)->train(BWAPI::UnitTypes::Protoss_Zealot);
			}
		}
	}
	for(iter = PositionsVisited.begin();iter != PositionsVisited.end();iter++)
	{
		Broodwar->drawCircle(CoordinateType::Map,(*iter).x(),(*iter).y(),150,Colors::Red,false);
	}
	if(reset >= 5)
	{
		PositionsVisited.clear();
	}
	if(Broodwar->getFrameCount()/24 - timeNewGat2 >= 60)
	{
		Buildings[gate] = 0;
	}
}

void Explore::onSendText(std::string text)
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
  }
}

void Explore::onReceiveText(BWAPI::Player* player, std::string text)
{
  Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void Explore::onPlayerLeft(BWAPI::Player* player)
{
  Broodwar->sendText("%s left the game.",player->getName().c_str());
}

void Explore::onNukeDetect(BWAPI::Position target)
{
  if (target!=Positions::Unknown)
    Broodwar->printf("Nuclear Launch Detected at (%d,%d)",target.x(),target.y());
  else
    Broodwar->printf("Nuclear Launch Detected");
}

void Explore::onUnitDiscover(BWAPI::Unit* unit)
{
	if(unit->getPlayer() == Broodwar->enemy())
	{
		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
		{
			if(!(*i)->getType().isWorker() && (*i)->getDistance(unit) < 300)
			{
				(*i)->attack(unit,false);
			}
		}
	}
}

void Explore::onUnitEvade(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] was last accessible at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void Explore::onUnitShow(BWAPI::Unit* unit)
{
	if(unit->getPlayer() == Broodwar->enemy())
	{
		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
		{
			if(!(*i)->getType().isWorker() && (*i)->getDistance(unit) < 300)
			{
				(*i)->attack(unit,false);
			}
		}
	}
}

void Explore::onUnitHide(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] was last seen at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void Explore::onUnitCreate(BWAPI::Unit* unit)
{
  if (Broodwar->getFrameCount()>1)
  {
    if (!Broodwar->isReplay())
      Broodwar->sendText("A %s [%x] has been created at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
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

void Explore::onUnitDestroy(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] has been destroyed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void Explore::onUnitMorph(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay())
    Broodwar->sendText("A %s [%x] has been morphed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
  else
  {
    /*if we are in a replay, then we will print out the build order
    (just of the buildings, not the units).*/
    if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
    {
      int seconds=Broodwar->getFrameCount()/24;
      int minutes=seconds/60;
      seconds%=60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
    }
  }
}

void Explore::onUnitRenegade(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay())
    Broodwar->sendText("A %s [%x] is now owned by %s",unit->getType().getName().c_str(),unit,unit->getPlayer()->getName().c_str());
}

void Explore::onSaveGame(std::string gameName)
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

void Explore::drawStats()
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

void Explore::drawBullets()
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

void Explore::drawVisibilityData()
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

void Explore::drawTerrainData()
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

void Explore::showPlayers()
{
  std::set<Player*> players=Broodwar->getPlayers();
  for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
  {
    Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
  }
}

void Explore::showForces()
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

void Explore::onUnitComplete(BWAPI::Unit *unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] has been completed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
  if(unit->getType() == UnitTypes::Protoss_Pylon)
  {
	  Buildings[pylo] = false;
  }
}
