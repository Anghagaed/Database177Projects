#include <iostream>
#include "RelOp.h"

using namespace std;


ostream& operator<<(ostream& _os, RelationalOp& _op) {
	return _op.print(_os);
}

Scan::Scan(Schema& _schema, DBFile& _file, string table) {
	schema = _schema;
	file = _file;
	table = table;
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
	return _os << "SCAN"<<schema<<" File";
}

Select::Select(Schema& _schema, CNF& _predicate, Record& _constants,
	RelationalOp* _producer, string table) {
	schema = _schema;
	predicate = _predicate;
	constants = _constants;
	producer = _producer;
	table = table;
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
	return _os << "SELECT"<<schema<<predicate<<constants.print(_os,schema)<<producer;
}


Project::Project(Schema& _schemaIn, Schema& _schemaOut, int _numAttsInput,
	int _numAttsOutput, int* _keepMe, RelationalOp* _producer) {
	_schemaIn = schemaIn;
	_schemaOut = schemaOut;
	_numAttsInput = numAttsInput;
	_numAttsOutput = numAttsOutput;
	_keepMe = keepMe;
	_producer = producer; 
}

Project::~Project() {

}

ostream& Project::print(ostream& _os) {
	return _os << "PROJECT: \nSchema In: " << schemaIn << "\nSchema Out: " << schemaOut << "\n# Attributes Input: " << numAttsInput << "\n# Attributes Output: " << numAttsOutput << "\nKeep: " << keepMe << "\nProducer: " << producer;
}


Join::Join(Schema& _schemaLeft, Schema& _schemaRight, Schema& _schemaOut,
	CNF& _predicate, RelationalOp* _left, RelationalOp* _right) {
	schemaLeft = _schemaLeft;
	schemaRight = _schemaRight;
	schemaOut = _schemaOut;
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
	return _os << "JOIN\nLeft Schema: " << schemaLeft << "\nRight Schema: " << schemaRight << "\nSchema Out: " << schemaOut << "\nPredicate: " << predicate << "\nLeft Operator: " << *left << "\nRight Operator: " << *right;
}


DuplicateRemoval::DuplicateRemoval(Schema& _schema, RelationalOp* _producer) {
	schema = _schema;
	producer = _producer;
}

DuplicateRemoval::~DuplicateRemoval() {

}

ostream& DuplicateRemoval::print(ostream& _os) {
	return _os << "DISTINCT \nSchema: " << schema << "\nProducer: " << *producer;
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

ostream& Sum::print(ostream& _os) {/*
	cout << "schemaIn = " << (_os, schemaIn) << endl;
	cout << "schemaOut = " << (_os,schemaOut) << endl;
	cout << "compute = " << compute.opList.recInput << " " << compute.numOps << " " << compute.returnInts << endl;
	cout << "producer = " << producer.print(_os) << endl;*/
	return _os << "SUM" << schemaIn << schemaOut << " Function" << producer;//told by TA to just use "Function"
}


GroupBy::GroupBy(Schema& _schemaIn, Schema& _schemaOut, OrderMaker& _groupingAtts,
	Function& _compute,	RelationalOp* _producer) {
	schemaIn = _schemaIn;
	schemaOut = _schemaOut;
	groupingAtts = _groupingAtts;
	compute = _compute;
	producer = _producer;
}

GroupBy::~GroupBy() {

}

ostream& GroupBy::print(ostream& _os) {/*
	cout << "schemaIn = " << (_os, schemaIn) << endl;
	cout << "schemaOut = " << (_os, schemaOut) << endl;
	cout << "groupingAtts = " << (_os,groupingAtts) << endl;
	cout << "compute = " << compute.opList.recInput << " " << compute.numOps << " " << compute.returnInts << endl;
	cout << "producer = " << producer.print(_os) << endl;*/
	return _os << "GROUP BY" << schemaIn << schemaOut << groupingAtts << " Function" << producer;//told by TA to just use "Function"
}


WriteOut::WriteOut(Schema& _schema, string& _outFile, RelationalOp* _producer) {
	_schema = schema;
	_outFile = outFile;
	_producer = producer;
}

WriteOut::~WriteOut() {

}

ostream& WriteOut::print(ostream& _os) {
	return _os << "OUTPUT\n Schema: " << schema << "\nOut File: " << outFile << "\n Producer: " << *producer;
}


ostream& operator<<(ostream& _os, QueryExecutionTree& _op) {
	return _os << "QUERY EXECUTION TREE";
}
