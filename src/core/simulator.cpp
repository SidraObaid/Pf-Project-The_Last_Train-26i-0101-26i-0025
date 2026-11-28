#include "simulator.h"
#include "physics.h"
#include "switches.h"
#include "logger.h"
#include "viewer_iface.h"
#include "util.h"
#include "level.h"
#include<iostream>
using namespace std;

// Main simulation loop
void runSimulation(int& playerFloor, int& playerRoom, int& energy, int& time, int& score, char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
    bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft, char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1], char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
    int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){
 int choice=0;
 int targetFloor=0;
 int targetRoom=0;
 char symbol=ENT_NONE;
 char code[MAX_NAME_LEN];
 // Main game loop
 while (checkEndConditions(energy, time, objectives)==false && choice!=9) {
  renderDashboard(playerFloor, playerRoom, energy, time, score, inventory, inventoryCount, emergencyTripsLeft);
  renderRoomDetails(playerFloor, playerRoom, roomEntities, entityCounts[playerFloor - 1][playerRoom - 1]);
  cout<<"1.Move 2.Inspect 3.Collect 4.Use 5.Unlock 6.Objectives 7.Save 8.Load 9.Quit"<<endl;
  cout<<"Enter choice:"<<endl;
  cin>>choice;
  // Run chosen action
  switch (choice) {
   case 1:
    cout<<"Enter floor and room:"<<endl;
    cin>>targetFloor>>targetRoom;
    handleMoveCommand(targetFloor, targetRoom, playerFloor, playerRoom, energy, time, horizontalConn, inventory, inventoryCount, emergencyTripsLeft, objectives[OBJ_LIFT2_ACTIVE]);
    break;
   case 2:
    handleInspectCommand(playerFloor, playerRoom, energy, time, roomEntities, entityCounts[playerFloor - 1][playerRoom - 1], objectives, score);
    break;
   case 3:
    cout<<"Enter item symbol to collect:"<<endl;
    cin>>symbol;
    handleCollectCommand(symbol, playerFloor, playerRoom, energy, time, score, roomEntities, entityCounts[playerFloor - 1][playerRoom - 1], inventory, inventoryCount, objectives);
    break;
   case 4:
    cout<<"Enter item symbol to use:"<<endl;
    cin>>symbol;
    handleUseCommand(symbol, playerFloor, playerRoom, energy, time, score, inventory, inventoryCount, objectives);
    break;
   case 5:
    cout<<"Enter cabinet code:"<<endl;
    cin>>code;
    handleUnlockCabinetCommand(code, playerFloor, playerRoom, objectives, score, roomEntities, entityCounts[playerFloor - 1][playerRoom - 1]);
    break;
   case 6:
    renderObjectivesChecklist(objectives);
    break;
   case 7:
    saveGameStateToFile("out/savegame.txt", playerFloor, playerRoom, energy, time, score, inventory, inventoryCount, objectives, emergencyTripsLeft, roomEntities, entityCounts);
    break;
   case 8:
    loadGameStateFromFile("out/savegame.txt", playerFloor, playerRoom, energy, time, score, inventory, inventoryCount, objectives, emergencyTripsLeft, roomEntities, entityCounts);
    break;
   case 9:
    cout<<"Quitting game."<<endl;
    break;
   default:
    cout<<"Invalid choice."<<endl;
    break;}
  // Clamp energy and time
  energy=clampInt(energy, 0, MAX_ENERGY);
  time=clampInt(time, 0, MAX_TIME);
  if (checkEndConditions(energy, time, objectives)==true) {
   break;}}
 // Write final report
 writeGameSummaryReport("out/game_summary.txt", score, energy, time, objectives, objectives[OBJ_MISSION_WON]);}

//execute move
bool handleMoveCommand(int targetFloor, int targetRoom, int& currentFloor, int& currentRoom, int& energy, int& time, const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                       const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, int& emergencyTripsLeft, bool lift2Operational){
 char connType=CONN_NONE;
 // Check lift connections
 if (isDirectlyConnected(currentFloor, currentRoom, targetFloor, targetRoom, horizontalConn, connType)!=true) {
  if (canUseLift2(currentFloor, targetFloor, lift2Operational, emergencyTripsLeft)==true && targetRoom==4) {
   connType='2';}
  else if (canUseLift1(currentFloor, targetFloor)==true) {
   connType='1';}}
 if (canTraverse(targetFloor, targetRoom, connType, inventory, inventoryCount, emergencyTripsLeft, lift2Operational)!=true) {
  cout<<"Cannot move."<<endl;
  return false;}
 //use emergency lift trip
 if (connType=='2') {
  if ((lift2Operational && hasItem(inventory, inventoryCount, ENT_LIFT_PASS))!=true) {
   emergencyTripsLeft--;
   removeItem(inventory, inventoryCount, ENT_EMERGENCY_CARD);}}
 currentFloor=targetFloor;
 currentRoom=targetRoom;

 if (connType=='1' || connType=='2') {
  applyResourceDelta(energy, time, COST_MOVE_LIFT_ENERGY, COST_MOVE_LIFT_TIME);}
 else if (connType==CONN_WINDOW) {
  applyResourceDelta(energy, time, COST_MOVE_WINDOW_ENERGY, COST_MOVE_WINDOW_TIME);}
 else {
  applyResourceDelta(energy, time, COST_MOVE_ORDINARY_ENERGY, COST_MOVE_ORDINARY_TIME);}
 cout<<"Moved to Floor "<<currentFloor<<", Room "<<currentRoom<<"."<<endl;
 return true;}

// Inspect
bool handleInspectCommand(int currentFloor, int currentRoom, int& energy, int& time, const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCount,
                          bool objectives[NUM_OBJECTIVES], int& score){
 applyResourceDelta(energy, time, COST_INSPECT_ENERGY, COST_INSPECT_TIME);
 if(entityCount==0){
  cout<<"There is nothing in this room."<<endl;
  return true;}
 cout<<"Items in this room:"<<endl;
 for(int i=0; i<entityCount; i++){
  char symbol=roomEntities[currentFloor-1][currentRoom-1][i];
  cout<<i<<". "<<getEntityName(symbol)<<endl;}
 // Check clue
 for(int i=0; i<entityCount; i++){
  char symbol=roomEntities[currentFloor-1][currentRoom-1][i];
  if(symbol==ENT_CLUE_4 && objectives[OBJ_CLUE_4]!=true){
   objectives[OBJ_CLUE_4]=true;
   applyScoreDelta(score, SCORE_CLUE_FOUND);
   cout<<"You found clue digit 4! (+"<<SCORE_CLUE_FOUND<<")"<<endl;}
  if(symbol==ENT_CLUE_2 && objectives[OBJ_CLUE_2]!=true){
   objectives[OBJ_CLUE_2]=true;
   applyScoreDelta(score, SCORE_CLUE_FOUND);
   cout<<"You found clue digit 2! (+"<<SCORE_CLUE_FOUND<<")"<<endl;}
  if(symbol==ENT_CLUE_7 && objectives[OBJ_CLUE_7]!=true){
   objectives[OBJ_CLUE_7]=true;
   applyScoreDelta(score, SCORE_CLUE_FOUND);
   cout<<"You found clue digit 7! (+"<<SCORE_CLUE_FOUND<<")"<<endl;}}
 return true;}

// collevct item
bool handleCollectCommand(char itemSymbol, int currentFloor, int currentRoom,
                          int& energy, int& time, int& score,
                          char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                          int& entityCount,
                          char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                          bool objectives[NUM_OBJECTIVES]){
 if (inventoryCount>=MAX_INVENTORY_CAPACITY) {
  cout<<"Inventory is full, item cant be collected"<<endl;
  return false;}
 for (int i=0; i<entityCount; i++) {
  if (roomEntities[currentFloor-1][currentRoom-1][i]==itemSymbol) {
   inventory[inventoryCount++]=itemSymbol;
   // Shift items left
   for (int j=i; j<entityCount-1; j++)
    roomEntities[currentFloor-1][currentRoom-1][j]=roomEntities[currentFloor-1][currentRoom-1][j+1];
   entityCount--;
   applyResourceDelta(energy, time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME);
   // Update objectives
   if (itemSymbol==ENT_MASTER_KEY) {
    objectives[OBJ_MASTER_KEY]=true;
    applyScoreDelta(score, SCORE_KEY_COLLECT);}
   else if (itemSymbol==ENT_FUSE) objectives[OBJ_FUSE_COLLECTED]=true;
   else if (itemSymbol==ENT_LIFT_PASS) {
    objectives[OBJ_LIFT_PASS]=true;
    applyScoreDelta(score, SCORE_LIFT_PASS);}
   else if (itemSymbol==ENT_CABINET_TRAIN_KEY) {
    objectives[OBJ_TRAIN_KEY]=true;
    applyScoreDelta(score, SCORE_TRAIN_KEY);}
   else if (itemSymbol==ENT_CAT_CARRIER) objectives[OBJ_CAT_CARRIER]=true;
   cout<<"Collected "<<getEntityName(itemSymbol)<<endl;
   return true;}}
 cout<<"Item not found in this room"<<endl;
 return false;}

// use item from inventory
bool handleUseCommand(char itemSymbol, int currentFloor, int currentRoom, int& energy, int& time, int& score, char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                      bool objectives[NUM_OBJECTIVES]){
 for (int i=0; i<inventoryCount; i++) {
  if (inventory[i]==itemSymbol) {
   // Install fuse on floor 2
   if (itemSymbol==ENT_FUSE && currentFloor==2 && currentRoom==1 && objectives[OBJ_FUSE_INSTALLED]!=true) {
    objectives[OBJ_FUSE_INSTALLED]=true;
    applyResourceDelta(energy, time, COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME);
    applyScoreDelta(score, SCORE_INSTALL_FUSE);}
   // Use energy pack item
   else if (itemSymbol==ENT_ENERGY_PACK) {
    applyResourceDelta(energy, time, BONUS_ENERGY_PACK, COST_ITEM_USE_TIME);}
   // Use medical kit item
   else if (itemSymbol==ENT_MEDICAL_KIT) {
    applyResourceDelta(energy, time, BONUS_ENERGY_PACK, COST_ITEM_USE_TIME);}
   // Activate lift 2 pass
   else if (itemSymbol==ENT_LIFT_PASS && objectives[OBJ_LIFT2_ACTIVE]!=true) {
    objectives[OBJ_LIFT2_ACTIVE]=true;
    applyResourceDelta(energy, time, COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME);
    applyScoreDelta(score, SCORE_LIFT_PASS);}
   // Secure stranded cat
   else if (itemSymbol==ENT_CAT_CARRIER && objectives[OBJ_CAT_SECURED]!=true) {
    objectives[OBJ_CAT_SECURED]=true;
    applyResourceDelta(energy, time, COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME);
    applyScoreDelta(score, SCORE_SECURE_CAT);}
   // Prepare train
   else if (itemSymbol==ENT_CABINET_TRAIN_KEY && objectives[OBJ_TRAIN_PREPARED]!=true) {
    objectives[OBJ_TRAIN_PREPARED]=true;
    objectives[OBJ_MISSION_WON]=true;
    applyResourceDelta(energy, time, COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME);
    applyScoreDelta(score, SCORE_PREPARE_TRAIN + SCORE_MISSION_VICTORY);}
   else {
    cout<<"Cannot use item here"<<endl;
    return false;}
   // Shift inventory left
   for (int j=i; j<inventoryCount-1; j++)
    inventory[j]=inventory[j+1];
   inventoryCount--;
   cout<<"Used "<<getEntityName(itemSymbol)<<endl;
   return true;}}
 cout<<"Item is not present in inventory"<<endl;
 return false;}

// unlock cabinet
bool handleUnlockCabinetCommand(const char* code, int currentFloor, int currentRoom, bool objectives[NUM_OBJECTIVES], int& score, char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                                int& entityCount){
 if (objectives[OBJ_CABINET_UNLOCKED]==true) {
  cout<<"Cabinet already unlocked."<<endl;
  return false;}
 if (verifyCabinetCode(code, objectives)==false) {
  cout<<"Cannot unlock cabinet."<<endl;
  return false;}
 objectives[OBJ_CABINET_UNLOCKED]=true;
 applyScoreDelta(score, SCORE_CABINET_UNLOCK);
 // train key
 if (entityCount<MAX_ENTITIES_PER_ROOM) {
  roomEntities[currentFloor-1][currentRoom-1][entityCount]=ENT_CABINET_TRAIN_KEY;
  entityCount++;}
 cout<<"Cabinet unlocked! Train Key available to pick."<<endl;
 return true;}

//end conditions
bool checkEndConditions(int energy, int time, const bool objectives[NUM_OBJECTIVES]){
 energy=clampInt(energy, 0, MAX_ENERGY);
 time=clampInt(time, 0, MAX_TIME);
 if(energy<=0)
  return true;
 else if(time<=0)
  return true;
 else if(objectives[OBJ_MISSION_WON]==true)
  return true;
 else
  return false;}
