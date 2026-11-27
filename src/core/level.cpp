#include "level.h"
#include "util.h"
#include "physics.h"
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;
bool loadBuildingMap(const char* filename,
                     char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                     char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                     int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]) {
    (void)filename; (void)horizontalConn; (void)roomEntities; (void)entityCounts;
    return false;   
}

bool loadInitialState(const char* filename,
                      int& playerFloor, int& playerRoom,
                      int& energy, int& time, int& score,
                      char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                      bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft) {
    (void)filename; (void)playerFloor; (void)playerRoom; (void)energy;
    (void)time; (void)score; (void)inventory; (void)inventoryCount;
    (void)objectives; (void)emergencyTripsLeft;
    return false;   
}

bool validateStartingRoom(int startFloor, int startRoom, const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1]) 
{
    (void)startFloor; (void)startRoom; (void)horizontalConn;
    return false;  
}

const char* getEntityName(char entitySymbol) 
{
    switch (entitySymbol)
    {
    case ENT_MASTER_KEY: 
    return "Master Key";
    case ENT_ENERGY_PACK:
    return "Energy Pack";
    case ENT_EMERGENCY_TOOLKIT:
    return "Emergency Toolkit";
    case ENT_CLUE_4:
    return "Clue Digit 4";
    case ENT_FUSE:
    return "Fuse";
    case ENT_CAT:
    return "stranded cat";
    case ENT_CONTROL_PANEL:
    return "Control Panel Terminal";
    case ENT_LIFT_PASS:
    return "Lift Pass";
    case ENT_PASSENGER:
    return "Stranded Passenger";
    case ENT_MEDICAL_KIT:
    return "Medical Kit";
    case ENT_EMERRGNCY_CARD:
    return "Emergency Access card";
    case ENT_CLUE_7:
    return "Clue Digit 7";
    case ENT_CLUE_2:
    return "Clue Digit 2";
    case ENT_CABINET_TRAIN_KEY:
    return "Cabinet/Train Key";
    case ENT_CAT_CARRIER:
    return "Cat Carrier";
    case ENT_OBSERVATION_TOKEN:
    return "Observation Token";
    default:
    return "Unknown";
    }  
}



















