/*
 * Author:	Hang V Liang
 * Purpose:	File for testing purposes
*/

#include "EfficientMap.h"
#include "EfficientMap.cc"
#include "Keyify.h"
#include "Keyify.cc"
#include "DataStructure.h"
#include <iostream>

using namespace std;
// Copying from Keyify.h to test how keyify works for various data type

class x {
public:
	EfficientMap< KeyString, tableInfo > m;
};

int main(int argc, char** argv) { 
	x test;
	cout << test.m.Length() << endl;
	KeyString key("testing");
	tableInfo data("test name", "test path", 5);
	test.m.Insert(key, data);
	cout << test.m.Length() << endl;
	return 0;
}
