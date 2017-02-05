#include <iostream>
#include "sqlite3.h"

#include "Schema.h"
#include "Catalog.h"
#include <sqlite3.h>
#include "DataStructure.h"
#include "DataStructure.cc"
#include "EfficientMap.h"
#include "EfficientMap.cc"
#include "Keyify.h"
#include "Keyify.cc"
using namespace std;

int Catalog::openDatabase(const char * _filename) {
	rc = sqlite3_open(_filename, &db);
	// Testing and printing error if it happens
	if ( rc != SQLITE_OK ) {
		cout << "Error opening database connection: " << endl;
		cout << sqlite3_errstr(rc) << endl;
	}
	// Returning the return code 
	return rc;
}

void Catalog::closeDatabase() {
	sqlite3_close(db);
}

void Catalog::query(const char * _sql) {
	rc = sqlite3_prepare_v2(db, _sql, -1, &stmt, NULL);
	if ( rc != SQLITE_OK ) {
		cout << "Error Preparing Query: " << endl;
		cout << sqlite3_errstr(rc) << endl;
	}
}

Catalog::Catalog(string& _fileName) {
	const int colNum = 0;
	openDatabase(_fileName.c_str());
	// MetaTables
	char * sql= "SELECT * FROM metaTables;";
	query(sql);
	rc = sqlite3_step(stmt);
	cout << endl << "Now at metaTables " << endl;
	while ( rc == SQLITE_ROW ) {	
		tableInfo pushData;
		
		// Getting the information from SQLITE
		// Organize in SQLITE in the following order:
		// table_name | data_Location | total_Tuples
		pushData.setName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		pushData.setPath(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
		pushData.setTuples(sqlite3_column_int(stmt, 2));
		KeyString pushKey(pushData.getName());
		
		// Testing printing purpose
		cout << pushData.getName() << " " << pushData.getPath() << " " << pushData.getTuples() << endl;
		
		// Pushing stuff into map
		tables.Insert(pushKey, pushData);
		// Step to new tuples if it exist
		rc = sqlite3_step(stmt);
	}
	
	cout << endl << "Testing if Map was successfully inserted" << endl;
	KeyString test("nation");
	if (tables.IsThere(test))
		cout<<"nations exist in Map"<<endl;
	KeyString kep("region");
	if (tables.IsThere(kep))
		cout<<"region exist in Map"<<endl;
	
	cout << endl << "Now at metaAttributes" << endl;
	cout << "Values of type: " << endl;
	cout << "Integer: " << Integer << endl;
	cout << "Float: " << Float << endl;
	cout << "String: " << String << endl;
	cout << "Name: " << Name << endl;
	sql = "SELECT * from metaAttributes;";
	query(sql);
	rc = sqlite3_step(stmt);
	while ( rc == SQLITE_ROW ) {
		Attribute pushData;
		
		// Getting the information from SQLITE
		// Organize in SQLITE in the following order:
		// table_name | attribute_name | type | total_distinct
		KeyString tableName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		pushData.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		pushData.noDistinct = sqlite3_column_int(stmt, 3);
		
		// Figure out which enum it falls under
		string test = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		
		if (test == "Integer") pushData.type = Integer;
		else if (test == "Float") pushData.type = Float;
		else if (test == "String") pushData.type = String;
		else if (test == "Name") pushData.type = Name;
		
		// Testing printing purpose
		cout << tableName << ": ";
		cout << pushData.name << " " << pushData.type << " " << pushData.noDistinct << endl;
		
		// Pushing attributes into schema
		if (tables.IsThere(tableName)) {
			tableInfo& toUse = tables.Find(tableName);
			Schema& schem = toUse.getSchema();
			Schema toPush(pushData);
			schem.append(Schema )
		}
		rc = sqlite3_step(stmt);
	}
}

Catalog::~Catalog() {
	
	closeDatabase();
}

bool Catalog::Save() {
	return true;
}

bool Catalog::GetNoTuples(string& _table, unsigned int& _noTuples) {
	/*if (!tables.IsThere(_table))
		return false;
	else
	{
		*_noTuples = tables.Find(_table).getTuples();
		return true;
	}*/
	return true;
}

void Catalog::SetNoTuples(string& _table, unsigned int& _noTuples) {
	/*if (!tables.IsThere(_table))
	{
		//Insert Error Message Here
		return;
	}
	else
		tables.Find(_table).setTuples(*_noTuples);*/
}

bool Catalog::GetDataFile(string& _table, string& _path) {
	/*if (!tables.IsThere(_table))
		return false;
	else
	{
		*_path = tables.Find(_table).getPath();
		return true;
	}*/
	return true;
}

void Catalog::SetDataFile(string& _table, string& _path) {
	/*if (!tables.IsThere(_table))
	{
		//Insert Error Message Here
		return;
	}
	else
		tables.Find(_table).setPath(*_path);*/
}

bool Catalog::GetNoDistinct(string& _table, string& _attribute,
	unsigned int& _noDistinct) {
	/*
	if (!atts.IsThere(_table))
		return false;
	else
	{
		*_noDistinct = atts.Find(_table).getDistinct();
		return true;
	}*/
	return true;
}
void Catalog::SetNoDistinct(string& _table, string& _attribute,
	unsigned int& _noDistinct) {
	/*if (!atts.IsThere(_table))
	{
		//Insert Error Message Here
		return;
	}
	else
		atts.Find(_table).setDistinct(*_noDistinct);*/
}

void Catalog::GetTables(vector<string>& _tables) 
{//do this, return by reference

	int i = 0;//used to traverse vector of strings called _tables
	this.MoveToStart();//set catalog iterator to starting position
	while (i<this.curDepth)//while iterator is less than depth of catalog
	{
		_tables.begin() + i = this.current.name;
		this.Advance();
		i++;
	}
	this.MoveToStart();//resetting the traverser to be nice

}

bool Catalog::GetAttributes(string& _table, vector<string>& _attributes)//assuming _table is already filled and we need to fill _attributes and _table is a key
{// do this

	bool check=false;
	int i = 0;
	while(i<this.curDepth)
	{
		if (this.begin + i == _table)
		{
			check=true;
			break;
		}
		i++;
	}
	if (check)
	{
		for (int i = 0; i < _attributes.size(); i++)
			this.attribute.push_back(_attributes.begin + i);
	}
	return check;

}

bool Catalog::GetSchema(string& _table, Schema& _schema) {
	return true;
}

bool Catalog::CreateTable(string& _table, vector<string>& _attributes, vector<string>& _attributeTypes) {
	
	int size = 100;
	String sqlArr[size];

	for (int i = 0; i < size; i++) {

		sql ="SELECT * FROM catalog.sqlitemaster WHERE type = 'table';
		ps = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	// count # of attributes in table
	int count = 0;	
	sqlite3_stmt* stmt;
	char* path = "catalog.db";

	// Part 1: Create Table 

	// code to grab all table names from catalog.db
	// if table already exists, do not create table
	if (_table == table) {
		return false;
	}
	// if table does not exist, create table
	else {


		sql = "CREATE TABLE " + _table + "(";

		// append to sql string for each table/ type 
		for (int i = 0; i < _attributes.size(); i++) {
			if (_attributeTypes == int || _attributeTypes = float) {
				sql += _attributes[i] + " " + _attributeTypes[i] + " (10000) NOT NULL";
				count++;
			}
			else if (_attributeTypes == String) {
				sql += _attributes[i] + " " + _attributeTypes[i] + " (500) NOT NULL";
				count++;
			}
		} 
		
		sql += ");";

		// Execute SQL statement 
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			cout << "SQL error" << endl;
		}
		else {
			return true;
		}
	}

	// Part 2: Create Table 

	// insert statement and push data into metaTables table in catalog 
	sqlTab = "INSERT INTO metaTables (t_name, dataLocation, totalTuples) " + 
			"VALUES (" + _table + ", TestData.db, + 0);";

	// insert statement and push data into metaAttributes table in catalog 
	// create new insert statement per attribute 
	for (int i = 0; i < count; i++) {
		sqlAtt = "INSERT INTO metaAttributes (t_name, a_name, type, totalDistinct) " +
			"VALUES (" + _table + ", ";
		sqlAtt += _attributes[i] + ", " + _attributeTypes[i] + ", 0);";
	}
	
	return true;
}

bool Catalog::DropTable(string& _table) {
	return true;
}

ostream& operator<<(ostream& _os, Catalog& _c) {
	return _os;
}
