#ifndef _DataStructure_H
#define _DataStructure_H
/*
 *	Author: 	Hang V Liang
 *	Purpose: 	Header file for the objects used in conjunction with the Catalog class.
 *			
 *
*/
# include <iostream>
# include "Schema.h"
# include "InefficientMap.h"
# include "EfficientMap.h"
# include "Config.h"
# include 
using namespace std;

// struct that is meant to hold a string
struct columnTuples { 
	Type type;							// Config.h describes the 4 possible types wrapped as an enum
	void * data;							// Void * to hold any generic data. Type cast using type when needed to decode
	
	// Constructor with overloading 
	columnTuples();							// Set Type and data to NULL
	columnTuples(Type type, void * data);				// Set type and data to the argument value;

	// Destructor
	~columnTuples();						// Free the memory stored in data if not set to NULL
};

// Objects with information regarding table name, schema, and all the attributes in it
// Tuples in a table will be partition and stored vertically. Example, if taple x has attributes (id, name) with tuples (1, "John")
// and (2, "Drake"). The tuples will be stored in two columnTuples, one storing all the id and one storing all the name.
// The efficient map will uses the key "id" to access the column
class tableInfo { 
private:
	Schema *schem;							// Schema pointer for a generic table
	char * name;							// Name of the table
	// Have not plan out how to efficiently stores the tuples. Using efficientmap as a stand-in 
	EfficientMap< char *, void *> tuples; 				// A map where key is the name of the attributes and the data is the tuples			
public:
};

// Objects that stores all the tables in a file
class tableStorage { 
private:
	EfficientMap< char *, tableInfo * > storage;			// A map where data is a tableInfo object pointers with the key being a string of the table name
public:

};



#endif // _DataStructure_H
