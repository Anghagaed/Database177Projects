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
	if (_groupingAtts != 0)									// Non-empty _groupingAtts -> GroupBy
	{
		// Create GroupBy here
		// Code here...
		
		// Create WriteOut here
		// Code here...
	}
	else
	if (_finalFunction != 0)								// Non-empty _finalFunction -> Sum
	{
		// Create Sum here
		// Code here...

		// Create WriteOut here
		// Code here...
	}
	else
	if (_distinctAtts == 0)									// _distinctAtts == 0 -> Project
	{
		// Create Project here
		// Code here...

		// Create WriteOut here
		// Code here...
	}
	else													// Else -> Project then DuplicateRemoval
	{
		// Create Project then create DuplicateRemoval
		// Code here...

		// Create WriteOut here
		// Code here...
	}
	// free the memory occupied by the parse tree since it is not necessary anymore
}
