#include "QueryCompiler.h"
#include "QueryOptimizer.h"
#include "Schema.h"
#include "ParseTree.h"
#include "Record.h"
#include "DBFile.h"
#include "Comparison.h"
#include "Function.h"
#include "RelOp.h"
#include <vector>
#include "EfficientMap.h"

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

void QueryCompiler::Compile(TableList* _tables, NameList* _attsToSelect,
	FuncOperator* _finalFunction, AndList* _predicate,
	NameList* _groupingAtts, int& _distinctAtts,
	QueryExecutionTree& _queryTree) {

	// create a SCAN operator for each table in the query
	Schema mySchema;
	string temp = _tables->tableName;
	catalog->GetSchema(temp, mySchema);
	DBFile myFile = DBFile();
	//char* path = "catalog.txt";
	myFile.Open("catalog.txt");
	Scan myScan = Scan(mySchema, myFile);
	// push-down selections: create a SELECT operator wherever necessary

	// call the optimizer to compute the join order
	OptimizationTree* root;
	optimizer->Optimize(_tables, _predicate, root);

	// create join operators based on the optimal order computed by the optimizer

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
