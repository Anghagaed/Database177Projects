/*
 * 	Author:		Hang V Liang
 *	Purpose:	Data Structure for Database Catalog to represent the catalog table
*/
#ifndef _DataStructure_H
#define _DataStructure_H
//# include "InefficientMap.h"
//# include "EffcientMap.h"
//# include "Keyify.h"
# include "Config.h"
# include <string>
# include <iostream>
using namespace std;

struct tableInfo { 
	string name;
	string path;
	int nuTuples;

	tableInfo(string na, string pa, int tu);
};

struct attsInfo { 
	string name;
	Type type;
	int disVal;

	attsInfo(string na, Type ty, int di);
};

/* Test to ensure it works
int main() {  
	
	tableInfo x("test", "NULL", 0);
	
	cout<<x.name<<endl;
	cout<<x.path<<endl;
	cout<<x.nuTuples<<endl;

	attsInfo y("test", Float, 5);

	cout<<y.name<<endl;
	cout<<y.type<<endl;
	cout<<y.disVal<<endl;

	return 0;
}
*/
#endif 
