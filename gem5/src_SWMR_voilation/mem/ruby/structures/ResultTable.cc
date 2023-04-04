/*

Written and Created By Sukarn 

*/

#include "mem/ruby/structures/ResultTable.hh"
#include <bits/stdc++.h> 
ResultTable* ResultTable::RT =  NULL;
int ResultTable:: m = 0;

ResultTable::ResultTable()
{
	addr = new uint64_t[m];
	val1 = new int[m];
	val2 = new int[m];
	val3 = new int[m];
	mid1 = new int[m];
	mid2 = new int[m];
	mid3 = new int[m];
	for (int i = 0; i < m; i++)
	{
		addr[i] = 0;
		val1[i] = 0;
		val2[i] = 0;
		val3[i] = 0;
		mid1[i] = -1;
		mid2[i] = -1;
		mid3[i] = -1;
	}
}



ResultTable* ResultTable :: getInstance(int numEntry)
{
	if(RT == NULL)
	{
		m = numEntry;
		RT = new ResultTable;
	}
		return RT;
}


void 
ResultTable::setAddr(uint64_t address, int index)
{
	addr[index] = address;
}


uint64_t
ResultTable::getAddr(int index)
{
	return addr[index];
}


void
ResultTable::setval1(int val, int index)
{
	val1[index] = val;
}

void
ResultTable::setval2(int val, int index)
{
	val2[index] = val;
}


void
ResultTable::setval3(int val, int index)
{
	val3[index] = val;
}

int
ResultTable::getval1(int index)
{
	return val1[index];
}
 
 
int
ResultTable::getval2(int index)
{
	return val2[index];
}

int 
ResultTable::getval3(int index)
{
	return val3[index];
}


void
ResultTable::setmid1(int val, int index)
{
	mid1[index] = val;
}

void 
ResultTable::setmid2(int val, int index)
{
	mid2[index] = val;
}

void
ResultTable::setmid3(int val, int index)
{
	mid3[index] = val;
}

int 
ResultTable::getmid1(int index)
{
	return mid1[index];
}

int 
ResultTable::getmid2(int index)
{
	return mid2[index];
}

int
ResultTable::getmid3(int index)
{
	return mid3[index];
}

bool
ResultTable::isFull()
{
	bool isFull = true;
	for (int i = 0; i < m; i++)
	{
		// Code for SB
		if(addr[i] != 0 && val2[i] != 0 && val1[i] != 0)
			continue;
		
		else
		{
			isFull = false;
			break;
		}	
	}
	
	return isFull;
}



bool
ResultTable::isFullIRIW()
{
	bool isFull = true;
				
	for (int i = 0; i < m; i++)
	{
		// Code for SB
		if(addr[i] != 0 && val1[i] != 0 && val2[i] != 0 && val3[i] != 0)
			continue;
		
		else
		{
			isFull = false;
			break;
		}	
	}
	
	return isFull;
}




uint64_t 
ResultTable::getBaseAddr(uint64_t prevAddr)
{
	uint64_t baseAddr = prevAddr;
	if(prevAddr == 0)
	{
		baseAddr = addr[0];
		for (int i = 0; i < m; i++)
		{
			uint64_t tempAddr = addr[i];
			if(tempAddr < baseAddr)
			{
				baseAddr = tempAddr;
			}
		}
	}
	else
	{
		int arr[m];
		for(int i = 0; i < m; i++)
		{
			arr[i] = addr[i];
		}
		std::sort(arr, arr+m);
		for(int i = 0; i < m; i++)
		{
			if(arr[i] == prevAddr)
			{
				if((i+1) < m)
				return arr[i+1];
			}	
		}		   			
	}
	return baseAddr;
}


int
ResultTable::getIndexByAddr(uint64_t address)
{
	int index = -1;
	for (int i = 0; i < m; i++)
	{
		if(address == addr[i])
		{
			index = i;
			break;
		}
	}
	return index;
}


void
ResultTable::printTable()
{
	std::cout << std::endl << "=====================================================================";
	for (int i = 0; i < m; i++)
	{
		std::cout << std::endl << "Address: " << addr[i] << "," <<"value: " << val1[i] << "," <<"value: " << val2[i] << "," <<"value: " << val3[i] << "mid1: " << mid1[i] << "," << "mid2: " << mid2[i] << "mid3: " << mid3[i];
		//std::cout << std::endl << "Address: " << addr[i] << "," <<"value: " << val1[i] << "," <<"value: " << val2[i];
	}
		std::cout << std::endl << "=====================================================================";
}


int 
ResultTable::getval1byMid(int index)
{
	//std::cout << std::endl << "mid1: " << mid1[index] << "," << "mid2: " << mid2[index] << "," <<"mid3: " << mid3[index];
	if(mid1[index] == mid2[index])
	{
		return val3[index];
	}
	else if(mid1[index] == mid3[index])
	{
		return val2[index];
	}
	else
	{
		return val1[index];
	}
}

int
ResultTable::getval2byMid(int index)
{
		//std::cout << std::endl << "mid1: " << mid1[index] << "," << "mid2: " << mid2[index] << "," <<"mid3: " << mid3[index];
	if(mid1[index] == mid2[index])
	{
		return val2[index];
	}
	else if (mid1[index] == mid3[index])
	{
		return val3[index];
	}
	else
	{
		return val3[index];
	}
}
