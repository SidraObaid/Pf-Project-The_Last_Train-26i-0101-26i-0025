#include "util.h"
#include "switches.h"
#include "physics.h"
using namespace std;
bool canUseLift1(int fromFloor, int toFloor)
{	//checks if lift 1 is usable
	if ((fromFloor ==1 && toFloor ==2) || (fromFloor ==2 && toFloor ==1))
		return true;
	else
		return false;
}
bool canUseLift2(int fromFloor, int toFloor, bool lift2Working, int emergencyTripsLeft)
{	//checks if lift 2 is usable
	bool from = (fromFloor ==2 || fromFloor ==4 ||fromFloor == 5);
	bool to = (toFloor ==2 || toFloor == 4 || toFloor == 5);
	if (!from || !to || fromFloor == toFloor)
		return false; 
	if (lift2Working)
		return true;
	return emergencyTripsLeft > 0;
}
bool unlockFloor3Door(const char inventory[MAX_INVENTORY_CAPACITY],  int inventoryCount)
{
	return hasItem(inventory, inventoryCount, ENT_MASTER_KEY);
}
bool installFuseControlPanel(char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, bool objectives[NUM_OBJECTIVES], int&score)
{	//installs the fusw in F3-R4
	if (objectives[OBJ_FUSE_INSTALLED])
		return false;
	if (!hasItem(inventory, inventoryCount, ENT_FUSE))
		return false;
	removeItem(inventory, inventoryCount, ENT_FUSE);
	objectives[OBJ_FUSE_INSTALLED] = true;
	applyScoreDelta(score, SCORE_INSTALL_FUSE);
	return true;
}
bool verifyCabinetCode(const char* enteredCode, const bool objectives[NUM_OBJECTIVES])
{	//opens cabinet if all 3 clues are found and correctly entered. If none entered returns false to prevent crashing.
	if (enteredCode == nullptr)
		return false;
	if (!objectives[OBJ_CLUE_4] || !objectives[OBJ_CLUE_2] || !objectives[OBJ_CLUE_7])
		return false;
	return stringEquals(enteredCode, "427");
}




























