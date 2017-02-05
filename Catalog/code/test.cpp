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
#include <string>

using namespace std;

class x {
public:
	EfficientMap< KeyString, tableInfo > m;
};

int main(int argc, char** argv) { 
/*
	x test;
	cout << test.m.Length() << endl;
	KeyString key("testing");
	tableInfo data("test name", "test path", 5);
	test.m.Insert(key, data);
	cout << test.m.Length() << endl;
*/
	tableInfo test("String1", "String2", 5);
	string table = "region", attribute, type;
	vector<string> attributes, types;
	vector<unsigned int> distincts;

	attribute = "r_regionkey"; attributes.push_back(attribute);
	type = "INTEGER"; types.push_back(type);
	distincts.push_back(5);
	attribute = "r_name"; attributes.push_back(attribute);
	type = "STRING"; types.push_back(type);
	distincts.push_back(5);
	attribute = "r_comment"; attributes.push_back(attribute);
	type = "STRING"; types.push_back(type);
	distincts.push_back(5);

	Schema s(attributes, types, distincts);
	test.setSchema(s);
	Schema kk;
	kk = test.getSchema();
	string testString = "r_regionkey";
	if (kk.GetDistincts(testString) == s.GetDistincts(testString)) 
		cout<<"testString 1: True"<<endl;
	return 0;
}
