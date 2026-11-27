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

bool validateStartingRoom(int startFloor, int startRoom,
                          const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1]) {
    (void)startFloor; (void)startRoom; (void)horizontalConn;
    return false;  
}

const char* getEntityName(char entitySymbol) {
    (void)entitySymbol;
    return "Unknown";   // TODO
}
