#include <iostream>
#include "sqlite3.h"

#include "Schema.h"
#include "Catalog.h"

using namespace std;

int Catalog::openDatabase(const char * _filename) {
	rc = sqlite3_open(_filename, &db);
	// Testing and printing error if it happens
	if ( rc != SQLITE_OK ) {
		cout << "Error opening database connection: " << endl;
		cout << sqlite3_errstr ( rv ) << endl;
	}
	// Returning the return code 
	return rc;
}

void Catalog::closeDatabase() {
	sqlite3_close(db);
}

void Catalog::Query(const char * _sql) {
	rc = sqlite3_prepare_v2(db, _sql, -1, &stmt, NULL);
	if ( rc != SQLITE_OK ) {
		cout << "Error Preparing Query: " << endl;
		cout << slqite3_errstr ( rc ) << endl;
	}
}

Catalog::Catalog(string& _fileName) {
	openDatabase(_fileName.c_str());
	// MetaTables

	// MetaAttributes

}

Catalog::~Catalog() {
	closeDatabase();
}

bool Catalog::Save() {
}

bool Catalog::GetNoTuples(string& _table, unsigned int& _noTuples) {
	return true;
}

void Catalog::SetNoTuples(string& _table, unsigned int& _noTuples) {
}

bool Catalog::GetDataFile(string& _table, string& _path) {
	return true;
}

void Catalog::SetDataFile(string& _table, string& _path) {
}

bool Catalog::GetNoDistinct(string& _table, string& _attribute,
	unsigned int& _noDistinct) {
	return true;
}
void Catalog::SetNoDistinct(string& _table, string& _attribute,
	unsigned int& _noDistinct) {
}

void Catalog::GetTables(vector<string>& _tables) {
}

bool Catalog::GetAttributes(string& _table, vector<string>& _attributes) {
	return true;
}

bool Catalog::GetSchema(string& _table, Schema& _schema) {
	return true;
}

bool Catalog::CreateTable(string& _table, vector<string>& _attributes, vector<string>& _attributeTypes) {
	
	/*int size = 100;
	String sqlArr[size];

	for (int i = 0; i < size; i++) {

		sql ="SELECT * FROM catalog.sqlitemaster WHERE type = 'table';
		ps = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);*/

	/**** Pseudocode 

	sqlite3_stmt* stmt;
	char* path = "catalog.db";

	// grab all table names from catalog.db
	// if table already exists, do not create table
	if (_table == table) {
		return false;
	}
	// if table does not exist, create table
	else {
		for (int i = 0; i < _attributes.size(); i++) {
			sql = "CREATE TABLE " + _table + "("
				+ _attributes[i] + " " + _attributeTypes[i] + ");";
		} // continue for all attributes 

		// Execute SQL statement 
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			cout << "SQL error" << endl;
		}
		else {
			return true;
		}
	}
	
	****/

}

bool Catalog::DropTable(string& _table) {
	return true;
}

ostream& operator<<(ostream& _os, Catalog& _c) {
	return _os;
}
