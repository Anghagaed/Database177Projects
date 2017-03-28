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
// Key is meant to contain integer, string, OR float.
// Since this is a struct, it can contain all at the same time,
// which is not the intention of this data structure. Please
// don't abuse that feature and use only one of them.
struct Key {
private:
	string stringKey;
	int intKey;		
	float floatKey;
public:
	Key() { stringKey = ""; intKey = 0; floatKey = 0.0f; }
	Key(int intKey) 
	{
		this->intKey = intKey;
	}
	Key(string stringKey)
	{
		this->stringKey = stringKey;
	}
	Key(float floatKey)
	{
		this->floatKey = floatKey;
	}

	// Keyify functions
	void Swap(Key& withMe)
	{
		SWAP(intKey, withMe.getIntKey());
		SWAP(floatKey, withMe.getFloatKey());
		SWAP(stringKey, withMe.getStringKey());
	}
	void CopyFrom(Key& withMe)
	{
		this->intKey = withMe.getIntKey();
		this->floatKey = withMe.getFloatKey();
		this->stringKey = withMe.getStringKey();
	}
	int IsEqual(Key &checkMe)										// assumes that both Keys have only one of them
	{
		bool intCheck = intKey == checkMe.getIntKey();
		bool floatCheck = floatKey == checkMe.getFloatKey();
		bool stringCheck = stringKey == checkMe.getStringKey();
		return intCheck && floatCheck && stringCheck;
	}
	int LessThan(Key &checkMe)										// assumes that both Keys have only one of them
	{
		bool intCheck = intKey < checkMe.getIntKey();
		bool floatCheck = floatKey < checkMe.getFloatKey();
		bool stringCheck = stringKey < checkMe.getStringKey();
		return intCheck || floatCheck || stringCheck;
	}

	// Object Interface
	int& getIntKey()
	{
		return intKey;
	}
	float& getFloatKey()
	{
		return floatKey;
	}
	string& getStringKey()
	{
		return stringKey;
	}
};

// Data
// Data is meant to contain integer sum OR float sum.
// Since this is a struct, it can contain both at the same time,
// which is not the intention of this data structure. Please
// don't abuse that feature and use only one of them.
struct Data {
private:
	int intSum;
	float floatSum;
public:
	// Constructors
	Data() { intSum = 0; floatSum = 0.0f; }	// default
	Data(float sum)							// float sum
	{
		floatSum = sum;
	}
	Data(int sum)							// int sum
	{
		intSum = sum;
	}

	// Keyify functions
	void Swap(Data& withMe)
	{
		SWAP(intSum, withMe.getIntSum());
		SWAP(floatSum, withMe.getFloatSum());
	}
	void CopyFrom(Data& withMe)
	{
		this->intSum = withMe.getIntSum();
		this->floatSum = withMe.getFloatSum();
	}
	
	// Object Interface
	int& getIntSum()						// returns intSum (assumes that this Data is intSum)
	{
		return intSum;
	}
	float& getFloatSum()					// returns floatSum (assumes that this Data is floatSum)
	{
		return floatSum;
	}
};

#endif