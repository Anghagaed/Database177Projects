/*
* 	Author:		Yun Chul Chang
*	Purpose:	Data Structure for Database GroupBy to represent the GroupBy tuples
*/
#ifndef _DataStruct_H
#define _DataStruct_H
# include "Config.h"
# include "Swap.h"
# include <iostream>
using namespace std;

// Stores the Key and Data of the GroupBy
// Compatible with EfficientMap

// Key
struct Key {
private:
	int keyInt;									// INCOMPLETE: Key only contains int (will contain more)
public:
	Key() { keyInt = 0; }
	Key(int keyInt) 
	{
		this->keyInt = keyInt;
	}

	// Keyify functions
	void Swap(Key& withMe)
	{
		SWAP(keyInt, withMe.getKey());
	}
	void CopyFrom(Key& withMe)
	{
		this->keyInt = withMe.getKey();
	}
	int IsEqual(Key &checkMe)
	{
		return keyInt == checkMe.getKey();
	}
	int LessThan(Key &checkMe)
	{
		return keyInt < checkMe.getKey();
	}

	// Object Interface
	int& getKey()
	{
		return keyInt;
	}
};

// Data
struct Data {
private:
	// 1 = int
	// 0 = float
	int whichSum;							// determines which type of data it is
	int intSum;
	float floatSum;
private:
	// some helper functions for Keyify...
public:
	Data() { whichSum = 0;  intSum = 0; floatSum = 0.0f; }
	Data(float sum, int whichSum)
	{
		this->whichSum = whichSum;
		if(this->whichSum == 1)				// int sum
			this->intSum = (int)sum;
		else								// float sum
			this->floatSum = sum;
	}

	// Keyify functions
	void Swap(Data& withMe)
	{
		SWAP(whichSum, withMe.whichType());
		SWAP(intSum, withMe.getIntSum());
		SWAP(floatSum, withMe.getFloatSum());
	}
	void CopyFrom(Data& withMe)
	{
		this->whichSum = withMe.whichType();
		this->intSum = withMe.getIntSum();
		this->floatSum = withMe.getFloatSum();
	}
	
	// Object Interface
	int& getIntSum()
	{
		return intSum;
	}
	float& getFloatSum()
	{
		return floatSum;
	}
	int& whichType()								// 1 = int; 0 = float
	{
		return whichSum;
	}
};

#endif