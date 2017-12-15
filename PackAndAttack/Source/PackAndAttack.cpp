#include "PackAndAttack.h"
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

int distance(int x1,int y1,int x2,int y2)
{
	int a = ((x1-x2)*(x1-x2));
	int b = ((y1-y2)*(y1-y2));
	return (int)sqrt((double)a+b);
}

Position MoveClosestBaseOrChokePoint(Unit *unit,std::set<BWTA::Chokepoint*> chokepoints)
{
	Position closest = Position(0,0);
	for(std::set<BWTA::Chokepoint*>::const_iterator chokes=chokepoints.begin();chokes!=chokepoints.end();chokes++)
	{
		if(closest == Position(0,0))
		{
			closest = (*chokes)->getCenter();
		}
		else if(distance(unit->getPosition().x(),unit->getPosition().y(),(*chokes)->getCenter().x(),(*chokes)->getCenter().y()) < distance(unit->getPosition().x(),unit->getPosition().y(),closest.x(),closest.y()))
		{
			closest = (*chokes)->getCenter();
		}
	}
	for(std::set<Unit *>::const_iterator bases=Broodwar->self()->getUnits().begin();bases!=Broodwar->self()->getUnits().end();bases++)
	{
		if((*bases)->getType().isResourceDepot())
		{
			if(distance(unit->getPosition().x(),unit->getPosition().y(),(*bases)->getPosition().x(),(*bases)->getPosition().y()) < distance(unit->getPosition().x(),unit->getPosition().y(),closest.x(),closest.y()))
			{
				closest = (*bases)->getPosition();
			}
		}
	}
	return closest;
}

bool analyzed2;
bool analysis_just_finished2;
BWTA::Region* home2;
BWTA::Region* enemy_base2;


int IdExplorador; // the explorer unit´s ID
BuildCell *BManager; // the manager that controls the buildings actions
int timeNewGat3; // the time that the last gateway was built
UnitType LastBuilt2; // the last building constructed
int LastBuildingPrice; // the price of the last building constructed 
bool build[3]; // the vector of boolean values that controls when to add a new building in the list

Position WhereToAttack; // position to attack the enemy
Position reunite; // the position to reunite the army
std::set<BWTA::Chokepoint*> chokepoints; // the chokepoints
std::set<Position>reunitePoints; //the vector of reunited positions
int i;


void PackAndAttack::onStart()
{
  IdExplorador = -1;
  WhereToAttack = Position(0,0);

  Broodwar->enableFlag(Flag::UserInput);

  BWTA::readMap();
  analyzed2=false;
  analysis_just_finished2=false;

  show_bullets=false;
  show_visibility_data=false;

  if (Broodwar->isReplay())
  {
    Broodwar->printf("The following players are in this replay:");
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
    for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
    {
      if ((*i)->getType().isWorker())
      {
		if(IdExplorador == -1)
		{
			IdExplorador = (*i)->getID();
			break;
		}
      }
    }
  }
}

void PackAndAttack::onEnd(bool isWinner)
{
  if (isWinner)
  {
    //log win to file
  }
}

void PackAndAttack::onFrame()
{
  if (show_visibility_data)
    drawVisibilityData();

  if (show_bullets)
    drawBullets();

  if (Broodwar->isReplay())
    return;

  drawStats();
  if (analyzed2 && Broodwar->getFrameCount()%30==0)
  {
    //order one of our workers to guard our chokepoint.
    for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
    {
      if ((*i)->getType().isWorker())
      {
        //get the chokepoints linked to our home region
        chokepoints= home2->getChokepoints();
        double min_length=10000;
        BWTA::Chokepoint* choke=NULL;

        //iterate through all chokepoints and look for the one with the smallest gap (least width)
        for(std::set<BWTA::Chokepoint*>::iterator c=chokepoints.begin();c!=chokepoints.end();c++)
        {
          double length=(*c)->getWidth();
          if (length<min_length || choke==NULL)
          {
            min_length=length;
            choke=*c;
          }
        }

        //order the worker to move to the center of the gap
        (*i)->rightClick(choke->getCenter());
        break;
      }
    }
  }
  if (analyzed2)
    drawTerrainData();

  if (analysis_just_finished2)
  {
    Broodwar->printf("Finished analyzing map.");
    analysis_just_finished2=false;
  }


  for(std::set<Unit *>::const_iterator a=Broodwar->self()->getUnits().begin();a!=Broodwar->self()->getUnits().end();a++)
  {
	  if(!(*a)->getType().isWorker() && (*a)->isIdle())
	  {
		 reunite = MoveClosestBaseOrChokePoint((*a),chokepoints);
		 if(reunite != Position(0,0))
		 {
			 reunitePoints.insert(reunite);
			 (*a)->move(reunite);
		 }
	  }
	  if((*a)->getType().isWorker())
	  {

	  }
  }
  if(WhereToAttack != Position(0,0))
  {
	  for(std::set<Position>::const_iterator i=reunitePoints.begin();i != reunitePoints.end();i++)
	  {
		  for(std::set<Unit *>::const_iterator a=Broodwar->getUnitsInRadius((*i),150).begin();a!=Broodwar->getUnitsInRadius((*i),150).end();a++)
		  {
			  	if(!(*a)->getType().isWorker() && (*a)->isIdle())
				{
					(*a)->attack(WhereToAttack);
				}
		  }
	  }
  }
}

void PackAndAttack::onSendText(std::string text)
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
    if (analyzed2 == false)
    {
      Broodwar->printf("Analyzing map... this may take a minute");
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread2, NULL, 0, NULL);
    }
  } else
  {
    Broodwar->printf("You typed '%s'!",text.c_str());
    Broodwar->sendText("%s",text.c_str());
  }
}

void PackAndAttack::onReceiveText(BWAPI::Player* player, std::string text)
{
  Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void PackAndAttack::onPlayerLeft(BWAPI::Player* player)
{
  Broodwar->sendText("%s left the game.",player->getName().c_str());
}

void PackAndAttack::onNukeDetect(BWAPI::Position target)
{
  if (target!=Positions::Unknown)
    Broodwar->printf("Nuclear Launch Detected at (%d,%d)",target.x(),target.y());
  else
    Broodwar->printf("Nuclear Launch Detected");
}

void PackAndAttack::onUnitDiscover(BWAPI::Unit* unit)
{
  if(Broodwar->self() != unit->getPlayer())
  {
	  if(unit->getType().isBuilding() && !unit->getType().isNeutral() && unit->getPlayer() == Broodwar->enemy())
	  {
		  WhereToAttack = unit->getPosition();
	  }
	  for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	  {
	  }
  }
}

void PackAndAttack::onUnitEvade(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] was last accessible at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void PackAndAttack::onUnitShow(BWAPI::Unit* unit)
{
  if(Broodwar->self() != unit->getPlayer())
  {
	  if(unit->getType().isBuilding() && !unit->getType().isNeutral() && unit->getPlayer() == Broodwar->enemy())
	  {
		  WhereToAttack = unit->getPosition();
	  }
	  for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	  {
	  }
  }
}

void PackAndAttack::onUnitHide(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] was last seen at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void PackAndAttack::onUnitCreate(BWAPI::Unit* unit)
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

void PackAndAttack::onUnitDestroy(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] has been destroyed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void PackAndAttack::onUnitMorph(BWAPI::Unit* unit)
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

void PackAndAttack::onUnitRenegade(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay())
    Broodwar->sendText("A %s [%x] is now owned by %s",unit->getType().getName().c_str(),unit,unit->getPlayer()->getName().c_str());
}

void PackAndAttack::onSaveGame(std::string gameName)
{
  Broodwar->printf("The game was saved to \"%s\".", gameName.c_str());
}

DWORD WINAPI AnalyzeThread2()
{
  BWTA::analyze();

  //self start location only available if the map has base locations
  if (BWTA::getStartLocation(BWAPI::Broodwar->self())!=NULL)
  {
    home2       = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
  }
  //enemy start location only available if Complete Map Information is enabled.
  if (BWTA::getStartLocation(BWAPI::Broodwar->enemy())!=NULL)
  {
    enemy_base2 = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
  }
  analyzed2   = true;
  analysis_just_finished2 = true;
  return 0;
}

void PackAndAttack::drawStats()
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

void PackAndAttack::drawBullets()
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

void PackAndAttack::drawVisibilityData()
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

void PackAndAttack::drawTerrainData()
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

void PackAndAttack::showPlayers()
{
  std::set<Player*> players=Broodwar->getPlayers();
  for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
  {
    Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
  }
}

void PackAndAttack::showForces()
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

void PackAndAttack::onUnitComplete(BWAPI::Unit *unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] has been completed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}
