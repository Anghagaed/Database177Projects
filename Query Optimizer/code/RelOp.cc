#include <iostream>
#include <fstream>
#include <sstream>			// used for var to str conversion
#include <string.h>			// used for memcpy and memmove
#include "RelOp.h"
#include "EfficientMap.h"
#include "EfficientMap.cc"
#include "Keyify.h"
#include "Config.h"
using namespace std;

ostream& operator<<(ostream& _os, RelationalOp& _op) {
	return _op.print(_os);
}
int counterEQ = 0;
void QueryExecutionTree::ExecuteQuery() {
	Record rec;
	// Delete Me
	//cout << "Execution Tree Counter: " << counterEQ++ << endl;
	while (root->GetNext(rec)) {	//call getNext of root until there are no more tuples
		//cout << "Execution Tree Counter: " << counterEQ++ << endl;
	}
}

Scan::Scan(Schema& _schema, DBFile& _file, string table) {
	schema = _schema;
	s = _schema;
	file = _file;
	this->table = table;
	counter = 0;
}

bool Scan::GetNext(Record& _record) {
	//cout << "Scan Counter: " << counter++ << endl;
	//if (counter > 100)
	//	exit(0);
	if(file.GetNext(_record)) {
		return true;
	}
	return false;
}

Scan::Scan()
{
	schema = Schema();
	file =  DBFile();
	counter = 0;
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

	counter = 0;
}

Select::Select()
{
	counter = 0;
}

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
	//Record record;
	//cout << "Select Counter: " << counter++ << endl;
	while (producer->GetNext(_record)) {
		if (predicate.Run(_record, constants)) {	// constants = literals?
			//_record = record;
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
	counter = 0;
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

bool DuplicateRemoval::GetNext(Record& _record)//compiles but is not finished
{
	Record recTemp; //to check if there are more records
	//OrderMaker(schema); //ordermaker used to execute Run for comparing
	vector <Record> duplTemp; //store the records in temporarily
	while (producer->GetNext(recTemp) == true)
	{
		int i = 0;	//for iteration
		bool check = true;	//for checking if duplicate
		while (i < duplTemp.size())
		{
			//if (OrderMaker(schema).Run(recTemp, duplTemp.begin() + i) ==0)
			//{
			//	check = false; //is a duplicate
			//}
			//else
			//{
			//	i++; //iterate
			//}
		}
		if (check) //check for duplicate
		{
			duplTemp.push_back(recTemp);	//push into vector
		}
		
	}
	return true;
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

bool Sum::GetNext(Record & _record)
{
	return false;
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


// NOW SUPPORTS ALL POSSIBLE GROUPING ATTRIBUTES AND SUMS
// KEY = Record(GROUPING ATTRIBUTE); DATA = SUM
// TEST WITH:
//		- Queries/Phase4Queries/11.sql for <int, float>
//		- Queries/Phase4Queries/12.sql for <string, float>
//		- Queries/Phase4Queries/13.sql for <float, float>
//		- Queries/Phase4Queries/14.sql for <int, int>
//		- Queries/Phase4Queries/15.sql for <string, int>
//		- Queries/Phase4Queries/16.sql for <float, int>
bool GroupBy::GetNext(Record& _record) {
	if (first)																	// check if this is called the first time
	{
		Record temp, projtemp;													// Records to insert
		int intResult = 0; double doubleResult = 0;										// for Function's Apply (computing sum)
		int returnsInt;															// to determine whether the sum type is int
		double sum;																// SUM to insert
		KeyDouble td;															// temp KeyDouble
		while (producer->GetNext(temp))
		{
			// Get the projected record (only grouping attributes)
			temp.SetOrderMaker(&groupingAtts);
			returnsInt = compute.Apply(temp, intResult, doubleResult);			// compute sum
			sum = intResult + doubleResult;										// get sum
			temp.Project(groupingAtts.whichAtts, groupingAtts.numAtts, schemaIn.GetNumAtts());
			projtemp = temp;													// extract only grouping attributes from Record
			td = KeyDouble(sum);
			if (map.IsThere(projtemp))											// the map has the record
				map.Find(projtemp) = KeyDouble(map.Find(projtemp)+sum);				// update sum
			else																// the map doesn't have the record
				map.Insert(projtemp, td);										// insert into map
			intResult = 0; doubleResult = 0;
		}
		first = false;															// set first time to false
	}

	// Print Records and Sums
	Schema test = schemaIn;
	vector<int> keepMe;
	for (int i = 0; i < groupingAtts.numAtts; ++i)
	{
		keepMe.push_back(groupingAtts.whichAtts[i]);
	}
	test.Project(keepMe);
	map.MoveToStart();
	while (!map.AtEnd())
	{
		map.CurrentKey().print(cout, test);
		cout << " << " << map.CurrentData() << "\n";
		map.Advance();
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
	counter = 0;
}

WriteOut::~WriteOut() {

}

bool WriteOut::GetNext(Record& _record) {
	//ofstream myOutputFile;								// Unn, no difference
	//myOutputFile.open(outFile.c_str());					// Unn, no difference

	// Delete Me
	//cout << "Write Out Counter: " << counter++ << endl;
	if (producer->GetNext(_record)) {
		_record.print(cout, schema);
		cout << endl;	
		//myOutputFile.close();								// Unn, no difference

		return true;
	}

	//myOutputFile.close();									// Unn, no difference
	return false;
}

ostream& WriteOut::print(ostream& _os) {
	return _os << "OUTPUT\nSchema: " << schema << "\nOut File: " << outFile << "\nProducer:\n{\n" << *producer << "\n}\n";
}


ostream& operator<<(ostream& _os, QueryExecutionTree& _op) {
	return _os << "QUERY EXECUTION TREE" << "(\n" << *_op.root << "\n)";
}
