/*
 *	Author: 	Hang V Liang:
 *	Purpose:	Implementation of DataStructure.h
 *
*/
#include "DataStructure.h"

using namespace std;

columnTuples::columnTuples(Type x, void* dat) : type(x), data(dat);

columnTuples::~columnTuples() { 
	if (data) { 
		delete data;
	}
}

tableInfo::tableInfo() : schem(0), name(""), uniqueCounter(0), list(0);

tableInfo::tableInfo(Schema* schema, char * str, int count) : schem(schema), name(str), uniqueCounter(count);

tableInfo::~tableInfo() { 
	if (schem) { 
		delete schem;
	}
	if (list) { 
		for (int i = 0; i < list->size(); ++i) { 
			if (list[i])
				delete list[i];
		}
	}
}
