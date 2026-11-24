#include "util.h"
// Primitive helper utilities, clamping, and screen control

#include <iostream>
#include <iomanip>
using namespace std;

int clampInt(int val, int minVal, int maxVal) {
if (val>maxVal) 
 val=maxVal;
else if (val<minVal) 
 val=minVal;
return val;
}
void trimWhitespace(char* str){
for (int i=0;str[i]!='\0'; i++){
 if (str[i]=='\r' || str[i]=='\n'){
  str[i]='\0';
  break;}}
}

bool stringEquals(const char* a, const char* b) {
  for (int i=0; a[i]!='\0'|| b[i]!='\0'; i++) {
   if (a[i]!=b[i]) 
    return false;}
  return true;}

void printHeader(const char* title){
 cout<<setw(50)<<title<<endl;}

void clearScreen(){
 for (int i = 0; i < 30; i++) 
        cout << endl;
}
