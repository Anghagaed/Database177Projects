#include "QueryCompiler.h"
#include "QueryOptimizer.h"
#include "Schema.h"
#include "ParseTree.h"
#include "Record.h"
#include "DBFile.h"
#include "Comparison.h"
#include "Function.h"
#include <vector>
#include "EfficientMap.h"
#include "EfficientMap.cc"
#include "Keyify.h"
#include "Keyify.cc"

using namespace std;


QueryCompiler::QueryCompiler(Catalog& _catalog, QueryOptimizer& _optimizer) :
	catalog(&_catalog), optimizer(&_optimizer) {
	catalog = &_catalog;
	optimizer = &_optimizer;
}

QueryCompiler::~QueryCompiler() {
	delete catalog;
	delete optimizer;
}
int QueryCompiler::tableSize(TableList* _tables)
{
	int size = 0;
	TableList* iterator;
	while (iterator != NULL)
	{
		iterator = iterator->next;
		size++;
	}
	return size;
}
void QueryCompiler::Compile(TableList* _tables, NameList* _attsToSelect,
	FuncOperator* _finalFunction, AndList* _predicate,
	NameList* _groupingAtts, int& _distinctAtts,
	QueryExecutionTree& _queryTree) {
	
	// create a SCAN operator for each table in the query
	int size = tableSize(_tables);
	for(int i = 0;  i < size; i++)
	{
		Schema mySchema;
		string temp = _tables->tableName;
		catalog->GetSchema(temp, mySchema);
		DBFile myFile = DBFile();
		string pathConvert = "catalog.txt";//going to be converted to char *
		char* path = new char[pathConvert.length() + 1];
		strcpy(path, pathConvert.c_str());
		myFile.Open(path);
		Scan myScan = Scan(mySchema, myFile);
		KeyString newKey(temp);
		ScanMap.Insert(newKey, myScan);
		_tables = _tables->next;
		// push-down selections: create a SELECT operator wherever necessary
		Record recTemp;
		CNF cnfTemp;
		if (cnfTemp.ExtractCNF(*_predicate, myScan.getSchema(), recTemp) == 0)// builds CNF and Record needed. Now we have Schema, Record, and CNF. Just need RelationOp
		{
			Select mySelect = Select(myScan.getSchema(), cnfTemp, recTemp, &myScan);
			SelectMap.Insert(newKey, mySelect);
		}
	}

	cout<<SelectMap.Length();

	// call the optimizer to compute the join order
	OptimizationTree* root;
	optimizer->Optimize(_tables, _predicate, root);

	
	// create join operators based on the optimal order computed by the optimizer
	// ONLY WORKS FOR LEFT-DEEP TREES CURRENTLY

	// create first join

	// get join predicate
	CNF predicate;
	Schema s1, s2;	//s1 will eventually be our final schema after appending everything in the right order
	catalog->GetSchema(root->tables[0], s1);	//get left schema
	catalog->GetSchema(root->tables[1], s2);	//get right schema
	predicate.ExtractCNF(*_predicate, s1, s2);	//get join predicate

	//get relational operator from scan/push-down select maps
	//get left relational op
	RelationalOp *left;
	KeyString key(root->tables[0]);
	if (SelectMap.IsThere(key)) {	//check if there was a push-down selection
		left = SelectMap.Find(key);
	}
	else if (ScanMap.IsThere(key)) {
		left = ScanMap.Find(key);
	}
	//get right relational op
	RelationalOp *right;
	KeyString key_right(root->tables[1]);
	if (SelectMap.IsThere(key_right)) {	//check if there was a push-down selection
		right = SelectMap.Find(key_right);
	}
	else if (ScanMap.IsThere(key)) {	//otherwise check for the scan
		right = ScanMap.Find(key);
	}

	Schema schema_temp = s1;	//save first schema
	s1.Append(s2);	//get schema after join operation

	//create join operator
	//will point to the root at the end
	Join *j = new Join(schema_temp, s2, s1, predicate, left, right);

	// go through ordered table of vectors produced by optimizer joining s1 with the next tables
	for (int i = 2; i < root->tables.size(); i++) {
		//get join predicate
		//s1 holds our left schema
		catalog->GetSchema(root->tables[i], s2);	//get right schema
		predicate.ExtractCNF(*_predicate, s1, s2);	//get join predicate

		//j is our left relational op
		//get the right table's relational op
		KeyString key(root->tables[i]);
		if (SelectMap.IsThere(key)) {	//check if there was a push-down selection
			right = SelectMap.Find(key);
		}
		else if (ScanMap.IsThere(key)) {	//otherwise check for the scan
			right = ScanMap.Find(key);
		}

		schema_temp = s1;	//save left schema
		s1.Append(s2);	//get schema out

		j = new Join(schema_temp, s2, s1, predicate, (RelationalOp*)j, right);	//join constructor takes in a relational op, so upcast j
	}
	//s1 is final schema
	//j points to the root of the join tree
	

	// create the remaining operators based on the query

	// connect everything in the query execution tree and return
	// Create WriteOut here
	/*
	WriteOut* writeout;
	*/
	if (_groupingAtts != 0)														// Non-empty _groupingAtts -> GroupBy
	{
		// Create GroupBy here
		/*
		Schema _schemaIn = ...;													// Set it equal to join's final schema
		Schema _schemaOut;														// Set it equal to nothing (not really important at the moment)
		vector<Attribute> att = _schemaIn.GetAtts();							// Get the Attributes
		int _atts_no = att.size();												// Get the Attribute size
		int _atts[_atts_no];													// Create an array of indices of Attributes
		for(int i = 0; i < att.size(); i++)
			_atts[i] = _schemaIn.Index(att[i]);									// Receive indices of all Attributes in the Schema
		OrderMaker _groupingAtts = new OrderMaker(_schemaIn, _atts, _atts_no);	// Insert all relevant values into OrderMaker
		Function _compute;
		_compute->GrowFromParseTree (_finalFunction, _schemaIn);				// Insert all relevant values into Function
		GroupBy* groupby = new GroupBy(_schemaIn, _schemaOut, _groupingAtts, _compute,	...);		// ... = Final join operator
		writeout = new WriteOut(_schemaOut, "", groupby);						// Insert all relevant values into WriteOut
																				// outFile is "" because we are not using it yet
		*/
	}
	else
	if (_finalFunction != 0)													// Non-empty _finalFunction -> Sum
	{
		// Create Sum here
		/*
		Schema _schemaIn = ...;													// Set it equal to join's final schema
		Schema _schemaOut;														// Set it equal to nothing (not really important at the moment)
		Function _compute;
		_compute->GrowFromParseTree (_finalFunction, _schemaIn);				// Insert all relevant values into Function
		Sum* sum = new Sum(_schemaIn, _schemaOut, _compute,	...);				// ... = Final join operator
		writeout = new WriteOut(_schemaOut, "", sum);							// Insert all relevant values into WriteOut
																				// outFile is "" because we are not using it yet
		*/
	}
	else																		// Project or Project + DuplicateRemoval
	{
		// Create Project here
		/*
		Schema _schemaIn = ...;													// Set it equal to join's final schema
		Schema _schemaOut;														// Set it equal to nothing (not really important at the moment)
		int _numAttsInput = _schemaIn.GetAtts().size();							// Get input size
		int _numAttsOutput = 0;													// Set it equal to 0 because we aren't doing that yet
		int _keepMe[_numAttsOutput];											// Same as above
		Project* project = new Project(_schemaIn, _schemaOut, _numAttsInput, _numAttsOutput, _keepMe, ...);		// ... = Final join operator 
		*/
		if (_distinctAtts != 0)													// _distinctAtts != 0 -> DuplicateRemoval
		{
			// Create DuplicateRemoval
			/*
			DuplicateRemoval* duplicateRemoval = new DuplicateRemoval(_schemaIn, project);	
			writeout = new WriteOut(_schemaIn, "", duplicateRemoval);			// Insert all relevant values into WriteOut
																				// outFile is "" because we are not using it yet
			*/
		}
		else
		{
			/*
			writeout = new WriteOut(_schemaIn, "", project);					// Insert all relevant values into WriteOut
																				// outFile is "" because we are not using it yet
			*/
		}
		// Create the QueryExecutionTree
		/*
		_queryTree = writeout;
		*/
	}
	// free the memory occupied by the parse tree since it is not necessary anymore
}
