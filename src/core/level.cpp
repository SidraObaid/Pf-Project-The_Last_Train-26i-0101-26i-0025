#include "level.h"
#include "util.h"
#include "physics.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;
static void roomConnector(const char* line, int floorNum, char connRow[ROOMS_PER_FLOOR-1])
{
	for (int slot=0; slot < ROOMS_PER_FLOOR-1; slot++)
	{
		//building floor names next to connector, then reading the connector and storing it in connRow[]
		char leftName[6] = {'F', (char)('0' + floorNum), '-', 'R', (char)('1' + slot), '\0'};
		char rightName[6] = {'F', (char)('0' + floorNum), '-', 'R', (char)('2' + slot), '\0'};
		const char* leftPos = strstr(line, leftName);
		const char* rightPos = strstr(line, rightName);
		if (leftPos == nullptr || rightPos == nullptr)
			continue;
		for (const char* p =leftPos +5; p< rightPos; p++)
		{
			if (*p == ' ')
				continue;
			if (*p == CONN_DOOR || *p == CONN_WINDOW || *p == CONN_SOLID_WALL || (*p == CONN_LOCKED_DOOR && *(p + 1) != '1'))
				connRow[slot] = *p;
			break;
		}
	}
}
static void readRoomEntities (const char* line, int floorIndex, char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
	{
		//records the entities inside each room.
		int room = -1;
		int len = (int)strlen(line);
		for (int col =8; col<len; col++)
		{
			char c = line[col];
			if (c == '|')
				room++;
			else if (c != ' ' && room>= 0 && room < ROOMS_PER_FLOOR)
			{
				int n = entityCounts[floorIndex][room];
				if (n < MAX_ENTITIES_PER_ROOM)
				{
					roomEntities[floorIndex][room][n] = c;
					entityCounts[floorIndex][room] = n+1;
				}
			}
		}
}
const int MAX_MAP_LINES = 60;
static int readAllLines(const char* filename, char lines[][MAX_LINE_LEN], int maxLines)
{	//open and read building.txt and put the lines into arrays
	ifstream in(filename);
	if (!in.is_open())
	{		
		cout << "Error: cannot open file building.txt";
		return -1;
	}
	int count =0;
	while (count < maxLines && in.getline(lines[count], MAX_LINE_LEN))
	{	//removes the \r at the end of the text files because I operate g++ on Windows
		int n = (int)strlen(lines[count]);
		if (n>0 && lines[count][n-1] == '\r')
			lines[count][n-1] = '\0';
		count++;
	}
	return count; 
}
static int findFloorLine(char lines[][MAX_LINE_LEN], int lineCount, int floorNum) 
{
    char tag[6] = {'F', (char)('0' + floorNum), '-', 'R', '1', '\0' };
    for (int i = 0; i < lineCount; i++) 
    {
        if (strstr(lines[i], tag) != nullptr)
        	return i;
    }
    return -1;
}
bool loadBuildingMap(const char* filename, char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1], char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]) 
{
	// initializing entities in each room to avoid garbage
	for (int f=0; f < NUM_FLOORS; f++)
	{
		for (int i=0; i< ROOMS_PER_FLOOR -1; i++)
			horizontalConn[f][i] = CONN_NONE;
		for (int j=0; j< ROOMS_PER_FLOOR; j++)
		{
			entityCounts[f][j] = 0;
			for (int k=0; k< MAX_ENTITIES_PER_ROOM; k++)
				roomEntities[f][j][k] = ENT_NONE;
		}
	}
	char lines[MAX_MAP_LINES][MAX_LINE_LEN];
	int lineCount = readAllLines(filename, lines, MAX_MAP_LINES);
	if (lineCount <=0)
		return false;
	for (int floorNum =1; floorNum<= NUM_FLOORS; floorNum++)
	{
		int labelLine = findFloorLine(lines, lineCount, floorNum);
		if (labelLine < 0 || labelLine +1 >= lineCount)
			return false;
		roomConnector(lines[labelLine], floorNum, horizontalConn[floorNum -1]);
		readRoomEntities(lines[labelLine +1], floorNum -1, roomEntities, entityCounts);
	}
	if (entityCounts[0][3] < MAX_ENTITIES_PER_ROOM)
	{
		roomEntities[0][3][entityCounts[0][3]] = ENT_ENERGY_PACK;
		entityCounts[0][3]++;
	}
	return true;
}
static const int MAX_PAIRS = 40;
static int readKeyVals(const char* filename, char keys[][MAX_NAME_LEN], char vals[][MAX_NAME_LEN], int maxPairs)
{	    //reads new states from initialstate.txt and stores them in 2 arrays for keys and vals
	ifstream in (filename);
	if (!in.is_open())
	{	
		cout << "Error: cannot open file initial-State.txt";
		return -1;
	}
	char line[MAX_LINE_LEN];
	int n=0;
	while (n < maxPairs && in.getline(line, MAX_LINE_LEN))
	{	//removes the \r at the end of each line, since the data files use Windows line endings
		int len = (int)strlen(line);
		if (len > 0 && line[len - 1] == '\r')
			line[len - 1] = '\0';
		//splitting the line into 2 wherever theres a colon with the '\0' terminator
		char* colon = strchr(line, ':');
		if (colon == nullptr)
			continue;
		*colon = '\0';
		strncpy(keys[n], line, MAX_NAME_LEN -1);
		keys[n][MAX_NAME_LEN -1] = '\0';
		strncpy(vals[n], colon +1, MAX_NAME_LEN -1);
		vals [n][MAX_NAME_LEN -1] = '\0';
		trimWhitespace(keys[n]);
		trimWhitespace(vals[n]);
		n++;
	}
	return n;
}
static const char* findVal(char keys[][MAX_NAME_LEN], char vals[][MAX_NAME_LEN], int n, const char* key)
{
	for (int i =0; i<n; i++)
	{
		if (stringEquals(keys[i], key))
			return vals[i];
	
	}
	return nullptr;
}
static bool isYes(const char* i)
{	//does not call stringEquals if i is a nullptr to prevent progrram from crashing.
	return i != nullptr && stringEquals (i, "YES");
}
bool loadInitialState(const char* filename, int& playerFloor, int& playerRoom,int& energy, int& time, int& score, char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft) 
{	//initializing all states to prevent garbage and start  a nwe game
	char keys[MAX_PAIRS][MAX_NAME_LEN], vals[MAX_PAIRS][MAX_NAME_LEN];
	int n = readKeyVals(filename, keys, vals, MAX_PAIRS);
	if ( n <=0)
		return false;
	energy = INITIAL_ENERGY;
	time = INITIAL_TIME;
	score = INITIAL_SCORE;
	inventoryCount = 0;
	emergencyTripsLeft = 0;
	for (int i=0; i < NUM_OBJECTIVES; i++)
		objectives[i] = false;
	for (int i=0; i< MAX_INVENTORY_CAPACITY; i++)
		inventory[i] = ENT_NONE;
	const char* v = findVal(keys, vals, n, "INITIAL_ENERGY");
	if (v != nullptr)
		energy = clampInt(atoi(v), 0, MAX_ENERGY);
	v = findVal(keys, vals, n, "INITIAL_TIME");
	if (v != nullptr)
		time = clampInt(atoi(v), 0, MAX_TIME);
	v = findVal(keys, vals, n, "INITIAL_SCORE");
	if (v != nullptr)
		score = atoi(v);
	objectives[OBJ_PASSENGER_RESCUED] = isYes(findVal(keys, vals, n, "PASSENGER_RESCUED"));
	objectives[OBJ_CAT_SECURED] = isYes(findVal(keys, vals, n, "CAT_RESCUED"));
	objectives[OBJ_CABINET_UNLOCKED] = isYes(findVal(keys, vals, n, "CABINET_UNLOCKED"));
	objectives[OBJ_TRAIN_KEY] = isYes(findVal(keys, vals, n, "TRAIN_KEY_COLLECTED"));
	objectives[OBJ_FUSE_INSTALLED] = isYes(findVal(keys, vals, n, "FUSE_INSTALLED"));
	objectives[OBJ_LIFT_PASS] = isYes(findVal(keys, vals, n, "LIFT_PASS_COLLECTED"));
	objectives[OBJ_TRAIN_PREPARED] = isYes(findVal(keys, vals, n, "TRAIN_PREPARED"));
	v = findVal(keys, vals, n, "CLUES_COLLECTED");
	if (v!= nullptr)
	{
		for (int i =0; v[i] != '\0'; i++)
		{
			if (v[i] == '4')
				objectives[OBJ_CLUE_4] = true;
			if (v[i] == '2')
				objectives[OBJ_CLUE_2] = true;
			if (v[i] == '7')
				objectives[OBJ_CLUE_7] = true;
				
		}
	}
	v = findVal(keys, vals, n, "INITIAL_INVENTORY");
	if (v != nullptr)
	{
		for (int i=0; v[i] != '\0'; i++)
		{
			if (!stringEquals(getEntityName(v[i]), "Unknown"))
				addItem(inventory, inventoryCount, v[i]);
		}
	}
	playerFloor =0; 
	playerRoom =0;
	v = findVal(keys, vals, n, "START_ROOM");
	if (v != nullptr && strlen(v) ==5 && v[0] == 'F' && v[2] == '-' && v[3] == 'R')
	{
		playerFloor = v[1] - '0';
		playerRoom = v[4] - '0';
	}
	return true;
}
bool validateStartingRoom(int startFloor, int startRoom, const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1])
{	//rejects a starting room that is outside the building
	(void)horizontalConn;
	if (startFloor < 1 || startFloor > NUM_FLOORS)
		return false;
	if (startRoom < 1 || startRoom > ROOMS_PER_FLOOR)
		return false;
	return true;
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
    case ENT_EMERGENCY_CARD:
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
