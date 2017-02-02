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

int main(int argc, char** argv) { 
	EfficientMap < KeyString, tableInfo > m;
	cout << m.Length() << endl;
	KeyString key("testing");
	tableInfo data("test name", "test path", 5);
	m.Insert(key, data);
	cout << m.Length() << endl;
	return 0;
}
