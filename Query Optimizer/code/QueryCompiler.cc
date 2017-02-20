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
	vector<Scan> scans;										// SCAN operator for each table? Better use a vector
	TableList* i_table = _tables;
	string datapath, tablename;
	Schema schema;
	while (i_table != 0)
	{
		tablename = i_table->tableName;						// get table name
		catalog.getSchema(tablename, schema);				// get schema
		catalog.getDataPath(tablename, datapath)			// get data path
		scans.push_back(Scan(schema, datapath));
	}
	// push-down selections: create a SELECT operator wherever necessary

	// call the optimizer to compute the join order
	OptimizationTree* root;
	optimizer->Optimize(_tables, _predicate, root);

	// create join operators based on the optimal order computed by the optimizer

	// create the remaining operators based on the query

	// connect everything in the query execution tree and return

	// free the memory occupied by the parse tree since it is not necessary anymore
}
