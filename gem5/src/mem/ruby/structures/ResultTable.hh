/* 


Written and Created by Sukarn Agarwal

*/ 

#include <iostream>
#include "mem/ruby/common/Address.hh"

class ResultTable {

private:
 static int m;
 static ResultTable * RT;
 uint64_t* addr;
 int* val1;
 int* val2;
 int* val3; // Variable added for IRIW
 int* mid1;
 int* mid2;
 int* mid3;
 ResultTable();
 
 
 public:
 static ResultTable* getInstance(int);
 void setAddr(uint64_t, int);
 uint64_t getAddr(int);
 void setval1(int, int);
 void setval2(int, int);
 void setval3(int, int);
 int getval1(int);
 int getval2(int);
 int getval3(int);
 void setmid1(int, int);
 void setmid2(int, int);
 void setmid3(int, int);
 int getmid1(int);
 int getmid2(int);
 int getmid3(int);
 bool isFull();
 bool isFullIRIW();
 int getval1byMid(int);
 int getval2byMid(int);
 uint64_t getBaseAddr(uint64_t);
 int getIndexByAddr(uint64_t);
 void printTable();
};
