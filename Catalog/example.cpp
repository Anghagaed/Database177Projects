/*
 * Author: 	Hang V Liang, Amar Saini (Formatting)
 * Code: 	Following code is an example of interfacing with a database using sqlite. Intended for usage when there someone forgot how to interface using sqlite.
*/
#include <iostream>
#include <sqlite3.h>


using namespace std;

sqlite3* db;	// Global Variable needed for interfacing functions from sqlite																	
int rv;		// Return Value to confirm if certain sqlite routine was successfully applied
char* sql;																			
char* errMsg = 0;


void query();
void openDatabase();
void closeDatabase();
void printResult(sqlite3_stmt* stmt, int iCol);

int main(int argc, char** argv) { 

	// Open Database Connection
	openDatabase();
	
	// Query (Preparing and Binding)
	query();
	
	// Close Database Connection
	closeDatabase();

}

void query() { 

	sqlite3_stmt* stmt;													// Used to create queries or statements

	sql = "SELECT * FROM region WHERE r_regionkey = ?;";				// Region has the following attributes: r_regionkey, r_name, r_comment
	
	cout << "Running the following query: " << endl << "SELECT * FROM region WHERE r_regionkey = ?;";

	// Creating Prepared Statement  
	rv = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);					// Refers to documentation for what the arguments are for
	
	if ( rv != SQLITE_OK ) { 											// Error Checking
		cout << "Error Preparing Query: " << endl;						// SQLITE_OK (0) is global constant from sqlite3.h defined to be no errors
		cout << sqlite3_errstr(rv) << endl;
		//return;
	}
	
	else { 
		cout << "Query prepared succesfully." << endl;
	}

	cout << "Binding '1' to the first ?" << endl;

	// Binding values to the r_regionkey which is a int
	rv = sqlite3_bind_int(stmt, 1, 1);
	if (rv != SQLITE_OK) { 
		cout << "Error Binding Value to Prepared Statement: " << endl;
		cout << sqlite3_errstr(rv) << endl;
		//return;
	}
	
	else { 
		cout<< "Query binded successfully." << endl;
	}

	// Printing result
	const int colNum = sqlite3_column_count(stmt);

	rv = sqlite3_step(stmt);

	while (rv == SQLITE_ROW) { 

		for (int i = 0; i < colNum; ++i) {

			printResult(stmt, i);

			if (i < colNum - 1)
				cout<<"|";
			
			else 
				cout<<endl;
		
		}

		rv = sqlite3_step(stmt);
	
	}

	sqlite3_finalize(stmt);

}

void printResult(sqlite3_stmt* stmt, int iCol) {

	int result = sqlite3_column_type(stmt, iCol);
	cout << sqlite3_column_name(stmt, iCol) << ": ";
	
	// All different data types that a column can have according to sqlite specification

	if (result == SQLITE_INTEGER) { 
		cout << sqlite3_column_int(stmt, iCol);
	}
	
	else if (result == SQLITE_FLOAT) { 
		cout << sqlite3_column_double(stmt, iCol);
	}
	
	else if (result == SQLITE_TEXT) { 
		cout << sqlite3_column_text(stmt, iCol);
	}
	
	else if (result == SQLITE_BLOB) { 
		// Not sure what a blob is
	}
	
	else if (result == SQLITE_NULL) { 
		cout << "NULL" << endl;
	}

}

void openDatabase() { 

	// Creating a database connection
	// Path will be local and in the same directory
	
	char* path = "TPCH.db";												// Path of the database file
	
	rv = sqlite3_open(path, &db);
	
	if ( rv != SQLITE_OK ) {											// SQLITE_OK (0) is global constant from sqlite3.h defined to be no errors  
		cout <<"Error opening database connection: "<< endl;		
		cout << sqlite3_errstr(rv) << endl;
	}
	
	else { 
		cout << "Database Open Successfully!" << endl;
	}

}

void closeDatabase() { 

	sqlite3_close(db);

}


