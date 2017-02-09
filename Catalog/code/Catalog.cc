#include <fstream>
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
	_filePath = _fileName;
	openDatabase(_filePath.c_str());
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

		// Pushing stuff into map
		tables.Insert(pushKey, pushData);
		// Step to new tuples if it exist
		rc = sqlite3_step(stmt);
	}
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
	closeDatabase();
}

Catalog::~Catalog() {
}

bool Catalog::Save() {
	
	openDatabase(_filePath.c_str());

	for (int i = 0; i < droppedTables.size(); i++) {

		KeyString tableKey(droppedTables[i]);

		if (!(tables.IsThere(tableKey))) {

			saveDrop(droppedTables[i]);

		}

	}

	tables.MoveToStart();

	while (!tables.AtEnd()) {

		if (tables.CurrentData().getAdd()) {

			saveAdd(tables.CurrentData().getName());

		}

		else if (tables.CurrentData().getChangedT()) {

			saveUpdate(tables.CurrentData().getName());

		}

		

	}

	tables.MoveToStart();

}

void Catalog::saveDrop(string& t_name) {
	KeyString key(t_name);
	tableInfo& toUse = tables.Find(key);
	Schema& schem = toUse.getSchema();
	
	char * sql = "DELETE FROM ";
}

void Catalog::saveAdd(string& t_name) {
	
}

void Catalog::saveUpdate(string& t_name) {
	KeyString key(t_name);
	tableInfo& toUse = tables.Find(key);
	Schema& schem = toUse.getSchema();
	if (toUse.getChangedT()) {
		char * sql = "UPDATE metaTables SET dataLocation = ?, totalTuples = ? WHERE t_name=?;";
		query(sql);
		
		sqlite3_bind_text(stmt, 1, (toUse.getPath()).c_str(), -1, NULL);
		sqlite3_bind_int(stmt, 2, toUse.getTuples());
		sqlite3_bind_text(stmt, 3, (toUse.getName()).c_str(), -1, NULL);
		
		rc = sqlite3_step(stmt);
	}
	vector<Attribute>& atts = schem.GetAtts();
	if (toUse.getChangedA()) {
		unsigned int size = schem.GetNumAtts();
		for (int i = 0; i < size; ++i) {
			if (atts[i].changed) {
				char * sql = "UPDATE metaAttributes SET totalDistinct = ? WHERE t_name=? AND a_name=?;";
				
				sqlite3_bind_int(stmt, 1, (atts[i].noDistinct));
				sqlite3_bind_text(stmt, 2, toUse.getName().c_str(), -1, NULL);
				sqlite3_bind_text(stmt, 3, atts[i].name.c_str(), -1, NULL);
				
				rc = sqlite3_step(stmt);
			}
		}
		
	}
}

bool Catalog::GetNoTuples(string& _table, unsigned int& _noTuples) {
	KeyString key(_table);
	if (!tables.IsThere(key)) {
		cout << "Error --- " << _table << " not found!" << endl;
		return false;
	}

	else
	{
		_noTuples = tables.Find(key).getTuples();
		return true;
	}
}

void Catalog::SetNoTuples(string& _table, unsigned int& _noTuples) {

	KeyString key(_table);
	if (!tables.IsThere(key))
	{
		cout << "Error --- " << _table << " not found!" << endl;
		return;
	}
	else {
		tables.Find(key).setTuples(_noTuples);
		tables.Find(key).setChangedT(true);
	}

}

bool Catalog::GetDataFile(string& _table, string& _path) {

	KeyString key(_table);
	if (!tables.IsThere(key)) {
		cout << "Error --- " << _table << " not found!" << endl;
		return false;
	}

	else
	{
		_path = tables.Find(key).getPath();
		return true;
	}
}

void Catalog::SetDataFile(string& _table, string& _path) {

	KeyString key(_table);
	if (!tables.IsThere(key))
	{
		cout << "Error --- " << _table << " not found!" << endl;
		return;
	}
	else {
		tables.Find(key).setPath(_path);
		tables.Find(key).setChangedT(true);
	}
		
}

bool Catalog::GetNoDistinct(string& _table, string& _attribute,
	unsigned int& _noDistinct) {
	KeyString key(_table);
	if (!tables.IsThere(key)) {
		cout << "Error --- " << _table << " not found!" << endl;
		return false;
	}

	else
	{
		_noDistinct = tables.Find(key).getSchema().GetDistincts(_attribute);
		return true;
	}
}
void Catalog::SetNoDistinct(string& _table, string& _attribute,
	unsigned int& _noDistinct) {
	KeyString key(_table);
	if (!tables.IsThere(key))
	{
		cout << "Error --- " << _table << " not found!" << endl;
		return;
	}
	else
	{
		int index = tables.Find(key).getSchema().Index(_attribute);
		tables.Find(key).getSchema().GetAtts()[index].noDistinct = _noDistinct;
		tables.Find(key).setChangedT(true);
		tables.Find(key).setChangedA(true);
		tables.Find(key).getSchema().GetAtts()[index].changed = true;
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
		vector<Attribute> fart = tables.Find(key).getSchema().GetAtts();
		for(int i = 0; i<fart.size(); i++)
		{
			_attributes.push_back(fart[i].name);
		}
			
		return true;
	}
	cout << "Error --- " << _table << " not found!" << endl;
	return false;
}

bool Catalog::GetSchema(string& _table, Schema& _schema) {

	KeyString key(_table);
	if (!tables.IsThere(key)) {
		cout << "Error --- " << _table << " not found!" << endl;
		return false;
	}

	else
	{
		_schema = tables.Find(key).getSchema();
		return true;
	}

	return true;
}

bool Catalog::CreateTable(string& _table, vector<string>& _attributes, vector<string>& _attributeTypes) {

	KeyString tableKey(_table);

	if (tables.IsThere(tableKey) == 1) {
		cout << "Error --- " << _table << " already exists!" << endl;
		return false;
	}


	tableInfo tableData;

	tableData.setName(_table);
	tableData.setTuples(0);
	tableData.setPath("New Path");

	vector<unsigned int> distincts;
	for (int i = 0; i < _attributes.size(); i++) {
		distincts.push_back(0);
	}


	Schema tableSchema(_attributes, _attributeTypes, distincts);

	tableData.setSchema(tableSchema);


	tables.Insert(tableKey, tableData);

	tables.Find(tableKey).setAdd(true);
	
	ofstream newFile;
	newFile.open("a.bin", ios::binary);
	if (newFile.is_open()) {
		cout << "YAY";
	}

	newFile.close();

	return true;

	}


bool Catalog::DropTable(string& _table) {

	KeyString tableKey(_table);

	if (tables.IsThere(tableKey) == 0) {
		cout << "Error --- " << _table << " not found!" << endl;
		return false;
	}

	KeyString tempK;
	tableInfo tempT;

	tables.Remove(tableKey, tempK, tempT);
	droppedTables.push_back(_table);

	return true;
}

ostream& operator<<(ostream& _os, Catalog& _c) {

	cout << endl;
	_os << "-------------------Printing Catalog-------------------" << endl;

	_c.tables.MoveToStart();

	while (!_c.tables.AtEnd()) {

		_os << "Table: " << _c.tables.CurrentData().getName() << "\t";
		_os << "noTuples: " << _c.tables.CurrentData().getTuples() << "\t";
		_os << "dataPath: " << _c.tables.CurrentData().getPath() << endl;

		vector<Attribute> atts = _c.tables.CurrentData().getSchema().GetAtts();

		for (int i = 0; i < atts.size(); i++) {

			cout << "\t" << "Attribute: " << atts[i].name << "\t";
			cout << "Type: " << convertType(atts[i].type) << "\t\t";
			cout << "noDistincts: " << atts[i].noDistinct << endl;

		}

		_c.tables.Advance();

	}

	_c.tables.MoveToStart();

	return _os;
}
