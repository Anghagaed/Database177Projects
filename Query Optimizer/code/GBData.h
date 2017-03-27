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
	float sum;							// INCOMPLETE: Data only contains float (will contain more)
public:
	Data() { sum = 0; }
	Data(float sum)
	{
		this->sum = sum;
	}

	// Keyify functions
	void Swap(Data& withMe)
	{
		SWAP(sum, withMe.getData());
	}
	void CopyFrom(Data& withMe)
	{
		this->sum = withMe.getData();
	}

	// Object Interface
	void setSum(float newsum)
	{
		sum = newsum;
	}
	float& getData()
	{
		return sum;
	}
};

#endif