#include "physics.h"
#include "util.h"

// Movement validation and resource adjustments
#include <iostream>
using namespace std;

// Check horizontal room
bool isDirectlyConnected(int fromF, int fromR, int toF, int toR, const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1], char& outConnType){
 if(fromF<1||fromF>NUM_FLOORS||toF<1||toF>NUM_FLOORS||fromR<1||fromR>ROOMS_PER_FLOOR||toR<1||toR>ROOMS_PER_FLOOR)
  return false;
 else if(fromF==toF&&fromR==toR)
  return false;
 else if(fromF!=toF&&fromR!=toR)
  return false;
 else if(fromF==toF){
  // Moving right one room
  if(fromR-toR==-1){
   outConnType=horizontalConn[fromF-1][fromR-1];
   // Block walls and empty
   if(outConnType==CONN_SOLID_WALL||outConnType==CONN_NONE){
    return false;}
   return true;}
  // Moving left one room
  else if(fromR-toR==1){
   outConnType=horizontalConn[fromF-1][toR-1];
   if(outConnType==CONN_SOLID_WALL||outConnType==CONN_NONE){
    return false;}
   return true;}}
return false;}

// is move is allowed?
bool canTraverse(int targetFloor, int targetRoom, char connType, const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, int emergencyTripsLeft, bool lift2Operational){
 if(targetFloor<1||targetFloor>NUM_FLOORS||targetRoom<1||targetRoom>ROOMS_PER_FLOOR)
  return false;
 if(connType==CONN_SOLID_WALL||connType==CONN_NONE)
  return false;
  else if(connType==CONN_LOCKED_DOOR){
  for(int i=0;i<inventoryCount;i++){
   if(inventory[i]==ENT_MASTER_KEY)
    return true;}
  return false;}
 // Lift 2 floor/room check
 else if(connType=='2'){
  if(targetRoom!=4||(targetFloor!=2 && targetFloor!=4 && targetFloor!=5))
   return false;
  if(lift2Operational==true)
   return true;
  // emergency card check
  if(emergencyTripsLeft>0){
   for(int i=0;i<inventoryCount;i++){
    if(inventory[i]==ENT_EMERGENCY_CARD)
     return true;}}
  return false;}
 // Lift 1 floor/room check
 else if(connType=='1'){
  if((targetFloor!=1 && targetFloor!=2)||(targetRoom!=2 && targetRoom!=3))
   return false;
  return true;}
 else
  return true;}

void applyResourceDelta(int& energy, int& time, int deltaEnergy, int deltaTime){
 energy=energy+deltaEnergy;
 energy=clampInt(energy,0,MAX_ENERGY);
 time=time+deltaTime;
 time=clampInt(time,0,MAX_TIME);}

void applyScoreDelta(int& score, int deltaScore){
 score=score+deltaScore;}

// Check item exists
bool hasItem(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, char itemSymbol){
 for(int i=0; i<inventoryCount;i++){
  if(inventory[i]==itemSymbol){
   return true;}}
 return false;}

// add item to inventory
bool addItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol){
 if(inventoryCount>=MAX_INVENTORY_CAPACITY)
  return false;
 inventory[inventoryCount]=itemSymbol;
 inventoryCount++;
 return true;}

// Remove and shift left
bool removeItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol){
 for(int i=0;i<inventoryCount;i++){
  if(inventory[i]==itemSymbol){
   
   for(int j=i;j<inventoryCount-1;j++)
    inventory[j]=inventory[j+1];
   inventoryCount--;
   return true;}}
 return false;}
