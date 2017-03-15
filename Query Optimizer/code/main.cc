#include <iostream>
#include <string>

#include "Catalog.h"
extern "C" {
#include "QueryParser.h"
}
#include "QueryOptimizer.h"
#include "QueryCompiler.h"
#include "RelOp.h"

using namespace std;


// these data structures hold the result of the parsing
extern struct FuncOperator* finalFunction; // the aggregate function
extern struct TableList* tables; // the list of tables in the query
extern struct AndList* predicate; // the predicate in WHERE
extern struct NameList* groupingAtts; // grouping attributes
extern struct NameList* attsToSelect; // the attributes in SELECT
extern int distinctAtts; // 1 if there is a DISTINCT in a non-aggregate query

extern "C" int yyparse();
extern "C" int yylex_destroy();

/*void loadData(String tabName, String heapLoc, String textLoc) {


}*/

int main()
{
	/*char result;
	String tabName;
	String heapLoc;
	String textLoc;

	cout << "Create heap files? y for yes, n for no: " << endl;
	cin >> result;

	while (result == 'y') {

		cout << "Enter the table name: " << endl;
		cin >> tabName;
		cout << "Enter the location of the heap file: " << endl;
		cin >> heapLoc;
		cout << "Enter the location of the text file: " << endl;
		cin >> textLoc;

		loadData(tabName, heapLoc, textLoc);

		cout << "Create more heap files? y for yes, n for no: " << endl;
		cin >> result;
	}*/

	bool quit = true;
	while (quit)
	{
		cout << "Enter a query and hit ctrl+D when done: " << endl;
		// this is the catalog
		string dbFile = "catalog.sqlite";
		Catalog catalog(dbFile);

		// this is the query optimizer
		// it is not invoked directly but rather passed to the query compiler
		QueryOptimizer optimizer(catalog);
		// this is the query compiler
		// it includes the catalog and the query optimizer
		QueryCompiler compiler(catalog, optimizer);


		// the query parser is accessed directly through yyparse
		// this populates the extern data structures
		int parse = -1;
		if (yyparse() == 0) {
			cout << "OK!" << endl;
			parse = 0;
		}
		else {
			//cout << "Error: Query is not correct!" << endl;
			parse = -1;
		}

		yylex_destroy();

		if (parse != 0) return -1;

		// at this point we have the parse tree in the ParseTree data structures
		// we are ready to invoke the query compiler with the given query
		// the result is the execution tree built from the parse tree and optimized
		QueryExecutionTree queryTree;
		compiler.Compile(tables, attsToSelect, finalFunction, predicate,
			groupingAtts, distinctAtts, queryTree);

		cout << queryTree << endl;

		queryTree.ExecuteQuery();

		/*OptimizationTree ptr;
		
		optimizer.getUniqueOrder(tables, predicate);
		optimizer.Optimize(tables, predicate, &ptr);*/
		cout << "Do you wish to continue? Press 0 to exit. Press any other number else to continue" << endl;
		int var;
		cin >> var;
		if (var == 0)
		{
			quit = false;
			break;
		}
	}
	return 0;

}