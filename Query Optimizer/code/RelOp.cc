#include <iostream>
#include <fstream>
#include <sstream>			// used for var to str conversion
#include <string.h>			// used for memcpy and memmove
#include "RelOp.h"
#include "EfficientMap.h"
#include "EfficientMap.cc"
#include "Keyify.h"
#include "Config.h"
#include "GBData.h"
#include "Keyify.h"
using namespace std;

int xyz = 0;

ostream& operator<<(ostream& _os, RelationalOp& _op) {
	return _op.print(_os);
}

void QueryExecutionTree::ExecuteQuery() {
	Record rec;
	while (root->GetNext(rec)) {	//call getNext of root until there are no more tuples
	}
}

Scan::Scan(Schema& _schema, DBFile& _file, string table) {
	schema = _schema;
	s = _schema;
	file = _file;
	this->table = table;
}

bool Scan::GetNext(Record& _record) {
	if(file.GetNext(_record)) {
		return true;
	}
	return false;
}

Scan::Scan()
{
	schema = Schema();
	file =  DBFile();
}

Scan::~Scan() {

}

Schema& Scan::getSchema()
{
	return schema;
}

DBFile& Scan::getFile()
{
	return file;
}

string Scan::getTable() {
	return table;
}

void Scan::Swap(Scan& withMe)
{
	SWAP(schema, withMe.getSchema());
	SWAP(file, withMe.getFile());
}

void Scan::CopyFrom(Scan& withMe)
{
	this->schema = withMe.schema;
	this->file = withMe.file;
}

ostream& Scan::print(ostream& _os) {
	return _os << "SCAN\nSchema:"<<schema<<"\nFile";
}

Select::Select(Schema& _schema, CNF& _predicate, Record& _constants,
	RelationalOp* _producer, string table) {
	schema = _schema;
	s = _schema;
	predicate = _predicate;
	constants = _constants;
	producer = _producer;
	this->table = table;

	count = 0;
}

Select::Select()
{}

Select::~Select() {

}

Schema& Select::getSchema()
{
	return schema;
}

CNF& Select::getCNF()
{
	return predicate;
}

Record& Select::getRecord()
{
	return constants;
}

RelationalOp* Select::getRelational()
{
	return producer;
}

string Select::getTable() {
	return table;
}

void Select::Swap(Select& withMe)
{
	SWAP(schema, withMe.schema);
	SWAP(predicate, withMe.predicate);
	SWAP(constants, withMe.constants);
	SWAP(producer, withMe.producer);
}

ostream& Select::print(ostream& _os) {
	return _os << "SELECT\nSchema: "<<schema<<"\nPredicate: "<<predicate<<"\nProducer: " << *producer << endl;
}

bool Select::GetNext(Record& _record) {
	Record record;
	while (producer->GetNext(record)) {
		if (predicate.Run(record, constants)) {	// constants = literals?
			_record = record;
			return true;
		}

	}
	return false;
}

Project::Project(Schema& _schemaIn, Schema& _schemaOut, int _numAttsInput,
	int _numAttsOutput, int* _keepMe, RelationalOp* _producer) {
	s = _schemaOut;
	schemaIn = _schemaIn;
	schemaOut = _schemaOut;
	numAttsInput = _numAttsInput;
	numAttsOutput = _numAttsOutput;
	keepMe = _keepMe;
	producer = _producer; 
}

Project::~Project() {

}

ostream& Project::print(ostream& _os) {
	_os << "PROJECT: \nSchema In: " << schemaIn << "\nSchema Out: " << schemaOut << "\n# Attributes Input: " << numAttsInput << "\n# Attributes Output: " << numAttsOutput << "\nKeep: ";
	for (int i = 0; i < numAttsOutput; i++) {
		_os << keepMe[i] << " ";
	}
	return _os << "\nProducer: " << *producer << endl;
}

bool Project::GetNext(Record& _record) {
	// Assume Project is working correctly
	// that is every private member variable is holding what it describes in header file
	if (producer->GetNext(_record)) {
		_record.Project(keepMe, numAttsOutput, numAttsInput);
		return true;
	}
	return false;
}

Join::Join(Schema& _schemaLeft, Schema& _schemaRight, Schema& _schemaOut,
	CNF& _predicate, RelationalOp* _left, RelationalOp* _right) {
	schemaLeft = _schemaLeft;
	schemaRight = _schemaRight;
	schemaOut = _schemaOut;
	s = _schemaOut;
	predicate = _predicate;
	left = _left;
	right = _right;
}

Join::~Join() {

}

Schema & Join::getSchema(){
	return schemaOut;
}

ostream& Join::print(ostream& _os) {
	return _os << "JOIN\nLeft Schema:\n" << schemaLeft << "\nRight Schema:\n" << schemaRight << "\nSchema Out:\n" << schemaOut << "\nPredicate:\n" << predicate << "\nLeft Operator:\n{\n" << *left << "\n}\nRight Operator:\n{\n" << *right << "\n}" << endl;
}


DuplicateRemoval::DuplicateRemoval(Schema& _schema, RelationalOp* _producer) {
	schema = _schema;
	producer = _producer;
}

DuplicateRemoval::~DuplicateRemoval() {

}

ostream& DuplicateRemoval::print(ostream& _os) {
	return _os << "DISTINCT \nSchema: " << schema << "\nProducer: " << *producer << endl;
}


Sum::Sum(Schema& _schemaIn, Schema& _schemaOut, Function& _compute,
	RelationalOp* _producer) {
	schemaIn = _schemaIn;
	schemaOut = _schemaOut;
	compute = _compute;
	producer = _producer;
}

Sum::~Sum() {
	
}

ostream& Sum::print(ostream& _os) {
	return _os << "SUM\nSchemaIn: " << schemaIn << "\nSchemaOut: " << schemaOut << " Function" << "Producer: " << *producer << endl;//told by TA to just use "Function"
}


GroupBy::GroupBy(Schema& _schemaIn, Schema& _schemaOut, OrderMaker& _groupingAtts,
	Function& _compute,	RelationalOp* _producer) {
	schemaIn = _schemaIn;
	schemaOut = _schemaOut;
	groupingAtts.Swap(_groupingAtts);
	compute = _compute;
	producer = _producer;
}

// Slow Variable to String Conversion Method
// Alternative Options (may require downloading):
// http://stackoverflow.com/questions/191757/how-to-concatenate-a-stdstring-and-an-int
template <typename T>
string convert(T x)
{
	ostringstream convert;   			// stream used for the conversion
	convert << x;		      			// insert the textual representation of 'Number' in the characters in the stream
	return convert.str(); 				// set 'Result' to the contents of the stream
}

// TEST WITH Queries/Phase4Queries/11.sql
// ONLY WORKS WITH int grouping attribute AND float SUM!!
// KEY = GROUPING ATTRIBUTE; DATA = SUM
bool GroupBy::GetNext(Record& _record) {
	// Assumes that there is only 1 grouping attributes
	// Assume that the map is <int, (int or float)>
	EfficientMap<Key, Data> map;
	Record temp;											// temp Record
	int column = groupingAtts.whichAtts[0];					// grouping attribute location
	int key;  char* data;  Key gbkey;						// temp Key (check GBData.h for see more details)
	Data gbdata; float newSumF; int newSumI; int dataType;	// temp Data (check GBData.h for see more details)
	int intResult;  double doubleResult; 					// for Function's Apply
	//int tempI; float tempF;								// temp int and double for Data 
	dataType = compute.GetSumType();						// receive SUM type from Function
	while (producer->GetNext(temp) == true)
	{
		data = temp.GetColumn(column);								// get grouping attribute value
		key = *((int*)data);										// cast to int
		gbkey = Key(key);											// create a key
		compute.Apply(temp, intResult, doubleResult);
		
		if (map.IsThere(gbkey))										// the key is inside the map
		{
			if (dataType == 1)										// SUM is int
			{
				newSumI = map.Find(gbkey).getIntSum() + intResult;	// compute new sum
				gbdata = Data(newSumI, dataType);					// convert to Data
			}
			else													// SUM is float
			{
				newSumF = map.Find(gbkey).getFloatSum() + (float)doubleResult;	// compute new sum
				gbdata = Data(newSumF, dataType);					// convert to Data
			}
			map.Find(gbkey) = gbdata;								// update the SUM
		}
		else														// the key is not inside the map
		{
			if (dataType == 1)										// SUM is int
			{
				newSumI = intResult;								// get the sum
				gbdata = Data(newSumI, dataType);					// convert to Data
			}
			else													// SUM is float
			{
				newSumF = (float)doubleResult;						// get the sum
				gbdata = Data(newSumF, dataType);					// convert to Data
			}
			map.Insert(gbkey, gbdata);								// insert into map
		}
	}
	map.MoveToStart();												// reset iterator
	/*
	// Test to ensure that there is some values inside the map
	while (!map.AtEnd())
	{
		cout << map.CurrentKey().getKey() << " << " << map.CurrentData().getData()<<"\n";
		map.Advance();
	}
	*/
	if (map.AtEnd())												// if we are at the end, then we are done return tuples so return false
		return false;
	else 
	{
		// Intention: Fill _record with the map
		// Current: Print records with the map. _record contains nothing
		// Work in progress...
		string s;
		string separator = convert('|');
		FILE* fp;
		int i = 1;
		char* result;
		int length1;
		int length2;
		while (!map.AtEnd())
		{
			//map.CurrentData().getData(tempI, tempF, dataType);	// get the current SUM
			if (dataType == 1)										// SUM is int
				s += convert(map.CurrentData().getIntSum());
			else													// SUM is float
				s += convert(map.CurrentData().getFloatSum());
			s += separator + convert(map.CurrentKey().getKey()) + separator;
			cout << s << endl;
			char* str = new char[s.length() + 1];
			strcpy(str, s.c_str());
			fp = fmemopen(str, s.length() * sizeof(char), "r");
			temp.ExtractNextRecord(schemaOut, *fp);
			temp.print(cout, schemaOut);
			cout << endl;
			/*
			if (_record.GetSize() > 0)
			{
				cout << "yes\n";
				length1 = _record.GetSize();
				length2 = temp.GetSize();
				result = new char[length1 + length2];					// array to hold the result.
				memcpy(result, _record.GetBits(), length1);					// copy memory _record into the result.
				memmove(result + length1, temp.GetBits(), length2);	// append memory temp record to the result.
				string c = result;
				cout << c << endl;
				_record.CopyBits(result, length1+length2);												// transfer result into _record
				_record.print(cout, schemaOut);
				cout << endl;
			}
			else
			{
				cout << "no\n";
				length2 = temp.GetSize();
				result = new char[length2];										// array to hold the result.
				memcpy(result, temp.GetBits(), length2);							// copy memory _record one into the result.
				_record.CopyBits(result, length2);												// transfer result into _record
				string c = result;
				cout << c << endl;
				_record.print(cout, schemaOut);
				cout << endl;
			}
			*/
			s = "";
			fclose(fp);
			delete str;
			map.Advance();
		}
	}
	return false;
}

GroupBy::~GroupBy() {

}

ostream& GroupBy::print(ostream& _os) {
	return _os << "GROUP BY\nSchemaIn: " << schemaIn << "\nSchemaOut: " << schemaOut << "\nGroupingAtts: " << groupingAtts << "\nFunction:\nFunction\n" << "Producer:\n{\n" << *producer << "\n}" << endl;//told by TA to just use "Function"
}


WriteOut::WriteOut(Schema& _schema, string& _outFile, RelationalOp* _producer) {
	schema = _schema;
	s = _schema;
	outFile = _outFile;
	producer = _producer;
}

WriteOut::~WriteOut() {

}

bool WriteOut::GetNext(Record& _record) {
	ofstream myOutputFile;
	myOutputFile.open(outFile.c_str());

	if (producer->GetNext(_record)) {
		_record.print(cout, schema);
		cout << endl;
		myOutputFile.close();

		return true;
	}

	myOutputFile.close();
	return false;
}

ostream& WriteOut::print(ostream& _os) {
	return _os << "OUTPUT\nSchema: " << schema << "\nOut File: " << outFile << "\nProducer:\n{\n" << *producer << "\n}\n";
}


ostream& operator<<(ostream& _os, QueryExecutionTree& _op) {
	return _os << "QUERY EXECUTION TREE" << "(\n" << *_op.root << "\n)";
}
