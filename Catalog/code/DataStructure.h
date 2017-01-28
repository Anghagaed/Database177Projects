#ifndef _DataStructure_H
#define _DataStructure_H
/*
 *	Author: 	Hang V Liang
 *	Purpose: 	Header file for the objects used in conjunction with the Catalog class.
 *			
 *
*/
# include <iostream>
# include <vector>

# include "Schema.h"
# include "InefficientMap.h"
# include "EfficientMap.h"
# include "Config.h"
# include "Keyify.h"
using namespace std;

// Typedef for int key
void IntSwap (int& a, int& b) { 
	int temp;
	temp = a;
	a = b;
	b = temp;
}
int IntCheck (int& a, int& b) { 
	return (a == b);
}
typedef Keyify < int, IntSwap, IntCheck > keyifiedInt;
// struct that is meant to hold Type and type of the data
struct columnTuples { 
	Type type;								// Config.h describes the 4 possible types wrapped as an enum
	void* data;								// Void * to hold any generic data. Type cast using type when needed to decode
	
	// Constructor with overloading 
	columnTuples(Type type, void* data);					// Set type and data to the argument value;

	// Destructor
	~columnTuples();							// Free the memory stored in data if not set to NULL
};
// Objects with information regarding table name, schema, and all the attributes in it
class tableInfo { 
private:
	Schema* schem;								// Schema pointer for a generic table
	string name;								// Name of the table
	int uniqueCounter;							// Counter used to ensure 
	vector < EfficientMap < keyifiedInt ,  columnTuples >* >* list;		// Orders of the attributes follows order in schema
public:
	// constructor
	tableInfo();								// Initialize everything to null, 0, and empty string
	tableInfo(Schema* schem, char* name, int count);			// Initialize with 3 arguments, list is set to null
	// destructor
	~tableInfo();
};

#endif // _DataStructure_H
