#ifndef _CATALOG_H
#define _CATALOG_H
/*
	makefile ADDED GCC:
	g++ DataStructure.h DataStructure.cc EfficientMap.h EfficientMap.cc Keyify.h Keyify.cc Config.h Swap.h Catalog.h Catalog.cc Schema.h Schema.cc main.cc -l sqlite3 -o a
*/
#include <string>
#include <vector>
#include <iostream>
#include <sqlite3.h>

#include "Schema.h"
#include "DataStructure.h"
#include "EfficientMap.h"
#include "Keyify.h"

using namespace std;


class Catalog {
private:
	/* Data structures to keep catalog data in memory.
	 * A series of data structures you may find useful are included.
	 * Efficient data structures are recommended.
	 * Avoid linear traversals when possible.
	 */
	 string _filePath;
	 EfficientMap<KeyString , tableInfo> tables;

	 vector<string> currentTables;
	 vector<string> droppedTables;
	 

	 /* SQLITE Interfacing parameters */
	 sqlite3 *db;
	 int rc;
	 char* errMsg;
	 sqlite3_stmt* stmt;
	 
	 /* SQLITE Interfacing Functions */
	 int openDatabase(const char * _filename);
	 void closeDatabase();
	 void query(const char * _sql);
	 
	 // Functions for save
	 void saveDrop(string& t_name);
	 void saveAdd(string& t_name);
	 void saveUpdate(string& t_name);
public:
	/* Catalog constructor.
	 * Initialize the catalog with the persistent data stored in _fileName.
	 * _fileName is a SQLite database containing data on tables and their attributes.
	 * _fileName is queried through SQL.
	 * Populate in-memory data structures with data from the SQLite database.
	 * All the functions work with the in-memory data structures.
	 */
	Catalog(string& _fileName);

	/* Catalog destructor.
	 * Store all the catalog data in the SQLite database.
	 */
	virtual ~Catalog();

	/* Save the content of the in-memory catalog to the database.
	 * Return true on success, false otherwise.
	 */
	bool Save();

	/* Get/Set the number of tuples in _table.
	 * Get returns true if _table exists, false otherwise.
	 */
	bool GetNoTuples(string& _table, unsigned int& _noTuples);
	void SetNoTuples(string& _table, unsigned int& _noTuples);

	/* Get/Set the location of the physical file containing the data.
	 * Get returns true if _table exists, false otherwise.
	 */
	bool GetDataFile(string& _table, string& _path);
	void SetDataFile(string& _table, string& _path);

	/* Get/Set the number of distinct elements in _attribute of _table.
	 * Get returns true if _table exists, false otherwise.
	 */
	bool GetNoDistinct(string& _table, string& _attribute, unsigned int& _noDistinct);
	void SetNoDistinct(string& _table, string& _attribute, unsigned int& _noDistinct);

	/* Return the tables from the catalog.
	 */
	void GetTables(vector<string>& _tables);

	/* Return the attributes of _table in _attributes.
	 * Return true if _table exists, false otherwise.
	 */
	bool GetAttributes(string& _table, vector<string>& _attributes);

	/* Return the schema of _table in _schema.
	 * Return true if _table exists, false otherwise.
	 */
	bool GetSchema(string& _table, Schema& _schema);

	/* Add a new table to the catalog with the corresponding attributes and types.
	 * The only possible types for an attribute are: INTEGER, FLOAT, and STRING.
	 * Return true if operation successful, false otherwise.
	 * There can be a single table with a given name in the catalog.
	 * There can be a single attribute with a given name in a table.
	 */
	bool CreateTable(string& _table, vector<string>& _attributes,
		vector<string>& _attributeTypes);

	/* Delete table from the catalog.
	 * Return true if operation successful, i.e., _table exists, false otherwise.
	 */
	bool DropTable(string& _table);

	/* Overload printing operator for Catalog.
	 * Print the content of the catalog in a friendly format:
	 * table_1 \tab noTuples \tab pathToFile
	 * \tab attribute_1 \tab type \tab noDistinct
	 * \tab attribute_2 \tab type \tab noDistinct
	 * ...
	 * table_2 \tab noTuples \tab pathToFile
	 * \tab attribute_1 \tab type \tab noDistinct
	 * \tab attribute_2 \tab type \tab noDistinct
	 * ...
	 * Tables/attributes are sorted in ascending alphabetical order.
	 */
	friend ostream& operator<<(ostream& _os, Catalog& _c);
};

#endif //_CATALOG_H
