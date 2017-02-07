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
//#include ""
using namespace std;

int Catalog::openDatabase(const char * _filename) {
	rc = sqlite3_open(_filename, &db);
	// Testing and printing error if it happens
	if (rc != SQLITE_OK) {
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
	if (rc != SQLITE_OK) {
		cout << "Error Preparing Query: " << endl;
		cout << sqlite3_errstr(rc) << endl;
	}
}

Catalog::Catalog(string& _fileName) {
	const int colNum = 0;
	openDatabase(_fileName.c_str());
	// MetaTables
	char * sql = "SELECT * FROM metaTables;";
	query(sql);
	rc = sqlite3_step(stmt);

	//cout << endl << "Now at metaTables " << endl;
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
		//cout << pushData.getName() << " " << pushData.getPath() << " " << pushData.getTuples() << endl;
		
		// Pushing stuff into map
		tables.Insert(pushKey, pushData);
		// Step to new tuples if it exist
		rc = sqlite3_step(stmt);
	}
	/*
	cout << endl << "Testing if Map was successfully inserted" << endl;
	KeyString nation("nation");
	if (tables.IsThere(nation))
		cout<<"nations exist in Map"<<endl;
	KeyString region("region");
	if (tables.IsThere(region))
		cout<<"region exist in Map"<<endl;
	
	cout << endl << "Now at metaAttributes" << endl;
	cout << "Values of type: " << endl;
	cout << "Integer: " << Integer << endl;
	cout << "Float: " << Float << endl;
	cout << "String: " << String << endl;
	cout << "Name: " << Name << endl;
	
	tableInfo& toUse1 = tables.Find(nation);
	Schema* schem1 = &(toUse1.getSchema());
	cout<<"Nations: "<<schem1->GetNumAtts()<<endl;
	tableInfo& toUse2 = tables.Find(region);
	Schema* schem2 = &(toUse2.getSchema());
	cout<<"Regions: "<<schem2->GetNumAtts()<<endl;
	*/
	sql = "SELECT * from metaAttributes;";
	query(sql);
	rc = sqlite3_step(stmt);
	while ( rc == SQLITE_ROW ) {
		
		// Getting the information from SQLITE
		// Organize in SQLITE in the following order:
		// table_name | attribute_name | type | total_distinct
		//tableName.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		vector<string> name, type;
		vector<unsigned int> distinct;
		KeyString tableName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		name.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
		distinct.push_back(sqlite3_column_int(stmt, 3));
		type.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
		rc = sqlite3_step(stmt);

		if (tables.IsThere(tableName)) {
			tableInfo& toUse = tables.Find(tableName);
			Schema& schem = toUse.getSchema();
			Schema toPush(name, type, distinct);
			//cout<<toPush<<endl;
			schem.Append(toPush);
		}
		name.clear();
		type.clear();
		distinct.clear();
	}
	/*
	cout<<"Nations: "<<schem1->GetNumAtts()<<endl;
	cout<<"Regions: "<<schem2->GetNumAtts()<<endl;
	cout<<toUse1.getSchema()<<endl;
	cout<<toUse2.getSchema()<<endl;
	*/
}

Catalog::~Catalog() {
	closeDatabase();
}

bool Catalog::Save() {
}

bool Catalog::GetNoTuples(string& _table, unsigned int& _noTuples) {
	KeyString key(_table);
	if (!tables.IsThere(key))
		return false;
	else
	{
		_noTuples = tables.Find(key).getTuples();
		return true;
	}
}

void Catalog::SetNoTuples(string& _table, unsigned int& _noTuples) {
	/*Keyify<string> key(_table);
	KeyString key(_table);
	if (!tables.IsThere(key))
	{
		cout << "Error: _table not found!" << endl;
		return;
	}
	else
		tables.Find(key).setTuples(_noTuples);*/
}

bool Catalog::GetDataFile(string& _table, string& _path) {
	/*Keyify<string> key(_table);
	KeyString key(_table);
	if (!tables.IsThere(key))
		return false;
	else
	{
		_path = tables.Find(key).getPath();
		return true;
	}*/
}

void Catalog::SetDataFile(string& _table, string& _path) {
	/*Keyify<string> key(_table);
	KeyString key(_table);
	if (!tables.IsThere(key))
	{
		cout << "Error: _table not found!" << endl;
		return;
	}
	else
		tables.Find(key).setPath(_path);*/
}

bool Catalog::GetNoDistinct(string& _table, string& _attribute,
	unsigned int& _noDistinct) {
	KeyString key(_table);
	if (!tables.IsThere(key))
		return false;
	else
	{
		Schema schema = tables.Find(key).getSchema();
		_noDistinct = schema.GetDistincts(_attribute);
		return true;
	}
}
void Catalog::SetNoDistinct(string& _table, string& _attribute,
	unsigned int& _noDistinct) {
	KeyString key(_table);
	if (!tables.IsThere(key))
	{
		cout << "Error: _table not found!" << endl;
		return;
	}
	else
	{
		Schema schema = tables.Find(key).getSchema();
		int index = schema.Index(_attribute);
		vector<Attribute> atts = schema.GetAtts();
		atts[index].noDistinct = _noDistinct;
	}
}

void Catalog::GetTables(vector<string>& _tables) 
{//do this, return by reference
	//catalog->efficientmap->datastructure
	tables.MoveToStart();//set catalog iterator to starting position
	while(!tables.AtEnd())
	{
		_tables.push_back(tables.CurrentData().getName());
		tables.Advance();
	}
	tables.MoveToStart();//resetting the traverser to be nice

}

bool Catalog::GetAttributes(string& _table, vector<string>& _attributes)//assuming _table is already filled and we need to fill _attributes and _table is a key
{// do this

	//int i = 0;
	
	KeyString key(_table);
	if (tables.IsThere(key))
	{
		tableInfo check = tables.Find(key);
		Schema temp = check.getSchema();
		vector<Attribute> fart = temp.GetAtts();
		for(int i = 0; i<fart.size(); i++)
		{
			_attributes.push_back(fart[i].name);
		}
			
		return true;
	}
	
	return false;
}

bool Catalog::GetSchema(string& _table, Schema& _schema) {
	return true;
}

bool Catalog::CreateTable(string& _table, vector<string>& _attributes, vector<string>& _attributeTypes) {
/*
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
	if (_attributeTypes == SQLITE_INTEGER || _attributeTypes = SQLITE_FLOAT) {
	sql += _attributes[i] + " " + _attributeTypes[i] + " (10000) NOT NULL";
	count++;
	}
	else if (_attributeTypes == SQLITE_TEXT) {
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

	// Part 2: Insert to Meta Table

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
	*/
	return true;

}

bool Catalog::DropTable(string& _table) {
	return true;
}

ostream& operator<<(ostream& _os, Catalog& _c) {
	return _os;
}
