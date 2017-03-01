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
}
int QueryCompiler::tableSize(TableList* _tables)
{
	int size = 0;
	TableList* iterator = _tables;
	while (iterator != NULL)
	{
		iterator = iterator->next;
		size++;
	}
	return size;
}

//goes through select/scan maps to get relational operator associated with table
RelationalOp* QueryCompiler::GetRelOp(string table) {
	RelationalOp *result;
	bool found = false;
	for (int i = 0; i < SelectMap.size(); i++) {
		if (table.compare(SelectMap[i].getTable())) {	//compare the string to what we have in our map
			result = &SelectMap[i];
			found = true;
		}
	}
	if (!found) {
		for (int i = 0; i < ScanMap.size(); i++) {
			if (table.compare(ScanMap[i].getTable())) {	//compare the string to what we have in our map
				result = &ScanMap[i];
				found = true;
			}
		}
	}
	return result;
}

/*
//Recursive function to traverse the optimization tree and create join operators along the way
//traverses in post order - reaches both children before parent
//returns the root of the join tree as a relational op pointer
//resulting schema can be accessed with the root's schemaOut
RelationalOp * QueryCompiler::JoinTree(OptimizationTree * node, AndList * _predicate) {
	RelationalOp* left;
	RelationalOp* right;
	if (node->leftChild != NULL) {
		left = JoinTree(node->leftChild, _predicate);	//get the relational op of left child
	}
	if (node->rightChild != NULL) {
		right = JoinTree(node->rightChild, _predicate);	//get the relational op of right child
	}
	RelationalOp *result;
	//check how many tables are at this node
	if (node->tables.size() == 1) {	//if it's not 2 or more, then search through our scan/select maps to find the relational op
		return GetRelOp(node->tables[0]);
	}

	//if there are 2 or more tables, then we need to join
	// gets the left/right schemas/relationalOp from the children of this node
	else {
		//make the join operator
		Schema left_schema;
		Schema right_schema;
		//get left schema somehow
		//downcast left into a scan/select/join and get the schema/schemaOut
		//get the type and then downcast accordingly
		string test = typeid(left).name();
		int n = test.size() - 2;
		if (test[n] == 'i') {	//check if it was a join
			left_schema = ((Join*)left)->getSchema();
		}
		else if (test[n] == 'c'){	//check if it was a select
			left_schema = ((Select*)left)->getSchema();
		}
		else if (test[n] == 'a') {	//check if it was a scan
			left_schema = ((Scan*)left)->getSchema();
		}

		//save left schema
		Schema left_temp = left_schema;

		//get right schema somehow
		//downcast left into a scan/select/join and get the schema/schemaOut???
		test = typeid(right).name();
		n = test.size() - 2;
		if (test[n] == 'i') {	//check if it was a join
			right_schema = ((Join*)right)->getSchema();
		}
		else if (test[n] == 'c') {	//check if it was a select
			right_schema = ((Select*)right)->getSchema();
		}
		else if (test[n] == 'a') {	//check if it was a scan
			right_schema = ((Scan*)right)->getSchema();
		}

		//get the predicate
		CNF predicate;
		predicate.ExtractCNF(*_predicate, left_schema, right_schema);

		//get resulting schema
		left_schema.Append(right_schema);	//leftschema now holds the resulting schema of join

		Join *j = new Join(left_temp, right_schema, left_schema, predicate, left, right);
		return j;	//return our relational op
	}
}
*/

void QueryCompiler::Compile(TableList* _tables, NameList* _attsToSelect,
	FuncOperator* _finalFunction, AndList* _predicate,
	NameList* _groupingAtts, int& _distinctAtts,
	QueryExecutionTree& _queryTree) {

	// create a SCAN operator for each table in the query
	int size = tableSize(_tables);
	TableList *amarlikesthepenis=_tables;
	for (int i = 0; i < size; i++)
	{
		Schema mySchema;
		string temp = amarlikesthepenis->tableName;
		//string temp = iterator->tableName;
		catalog->GetSchema(temp, mySchema);
		DBFile myFile = DBFile();
		string pathConvert = "catalog.txt";//going to be converted to char *
		char* path = new char[pathConvert.length() + 1];
		strcpy(path, pathConvert.c_str());
		myFile.Open(path);
		Scan *myScan = new Scan(mySchema, myFile, temp);
		ScanMap.push_back(*myScan);
		// push-down selections: create a SELECT operator wherever necessary
		Record recTemp;
		CNF cnfTemp;
		cnfTemp.ExtractCNF(*_predicate, myScan->getSchema(), recTemp);
		Select *mySelect = new Select(myScan->getSchema(), cnfTemp, recTemp, myScan, temp);
		//cout << (mySelect->getCNF()) << endl;
		if ((mySelect->getCNF()).numAnds!=0)// builds CNF and Record needed. Now we have Schema, Record, and CNF. Just need RelationOp
		{
			SelectMap.push_back(*mySelect);
		}
		amarlikesthepenis = amarlikesthepenis->next;
	}

	for (int k = 0; k < ScanMap.size(); k++)
	{
		cout << ScanMap[k] << "\n\n" << endl;
	}

	// Should only print 1: (supplier)
	for (int j = 0; j < SelectMap.size(); j++)
	{
		cout << SelectMap[j] << "\n\n" << endl;
	}

	// call the optimizer to compute the join order
	OptimizationTree* root;
	optimizer->Optimize(_tables, _predicate, root);
	/*
	// create join operators based on the optimal order computed by the optimizer
	Join* j;	//j will point to root of join tree
	if (size > 8) {
		// create join operators
		// only for left deep trees
		//will go through the vector of tables in order making join operators along the way

		// create first join

		// get join predicate
		CNF predicate;
		Schema s1, s2;	//s1 will eventually be our final schema after appending everything in the right order
		catalog->GetSchema(root->tables[0], s1);	//get left schema
		catalog->GetSchema(root->tables[1], s2);	//get right schema
		predicate.ExtractCNF(*_predicate, s1, s2);	//get join predicate

		//get relational operator from scan/push-down select maps
		//get left relational op
		RelationalOp *left = GetRelOp(root->tables[0]);
		//get right relational op
		RelationalOp *right = GetRelOp(root->tables[1]);

		Schema schema_temp = s1;	//save first schema
		s1.Append(s2);	//get schema after join operation

		//create join operator
		//will point to the root at the end
		j = new Join(schema_temp, s2, s1, predicate, left, right);

		// go through ordered table of vectors produced by optimizer joining s1 with the next tables
		for (int i = 2; i < root->tables.size(); i++) {
			//get join predicate
			//s1 holds our left schema
			catalog->GetSchema(root->tables[i], s2);	//get right schema
			predicate.ExtractCNF(*_predicate, s1, s2);	//get join predicate

			//j is our left relational op
			//get the right table's relational op
			right = GetRelOp(root->tables[i]);

			schema_temp = s1;	//save left schema
			s1.Append(s2);	//get schema out

			j = new Join(schema_temp, s2, s1, predicate, j, right);
		}
		
		//s1 is final schema
		//j points to the root of the join tree
	}
	else {
		//this should make the join tree for any general tree, not just left-deep
		j = (Join*)JoinTree(root, _predicate);	//need to cast relationalop to join somehow
	}
	*/
	// create the remaining operators based on the query

	// connect everything in the query execution tree and return
	// Create WriteOut here
	/*
	WriteOut* writeout;
	*/
	// ASSUME: Final Join's Schema = s1; Final Join's Pointer = j
	if (_groupingAtts != 0)														// Non-empty _groupingAtts -> GroupBy
	{
		// Create GroupBy here
		/*
		Schema _schemaIn = s1;												// Set it equal to join's final schema
		Schema _schemaOut;														// Set it equal to nothing (not really important at the moment)
		vector<Attribute> att = _schemaIn.GetAtts();							// Get the Attributes
		int _atts_no = att.size();												// Get the Attribute size
		int _atts[_atts_no];													// Create an array of indices of Attributes
		for(int i = 0; i < att.size(); i++)
			_atts[i] = _schemaIn.Index(att[i].name);							// Receive indices of all Attributes in the Schema
		OrderMaker _groupingAtts = OrderMaker(_schemaIn, _atts, _atts_no);		// Insert all relevant values into OrderMaker
		Function _compute;
		_compute.GrowFromParseTree (_finalFunction, _schemaIn);					// Insert all relevant values into Function
		GroupBy* groupby = new GroupBy(_schemaIn, _schemaOut, _groupingAtts, _compute,	j);		// j = Final join operator
		string outFile = "outfile";
		writeout = new WriteOut(_schemaOut, outFile, groupby);						// Insert all relevant values into WriteOut
																					// outFile is "outfile" because we are not using it yet
		*/
	}
	else
	if (_finalFunction != 0)													// Non-empty _finalFunction -> Sum
	{
		// Create Sum here
		/*
		Schema _schemaIn = s1;													// Set it equal to join's final schema
		Schema _schemaOut;														// Set it equal to nothing (not really important at the moment)
		Function _compute;
		_compute.GrowFromParseTree (_finalFunction, _schemaIn);				// Insert all relevant values into Function
		Sum* sum = new Sum(_schemaIn, _schemaOut, _compute,	j);				// j = Final join operator
		string outFile = "outfile";
		writeout = new WriteOut(_schemaOut, outFile, sum);							// Insert all relevant values into WriteOut
																					// outFile is "outfile" because we are not using it yet
		*/
	}
	else																		// Project or Project + DuplicateRemoval
	{
		// Create Project here
		/*
		Schema _schemaIn = s1;													// Set it equal to join's final schema
		Schema _schemaOut;														// Set it equal to nothing (not really important at the moment)
		int _numAttsInput = _schemaIn.GetAtts().size();							// Get input size
		int _numAttsOutput = 0;													// Set it equal to 0 because we aren't doing that yet
		int _keepMe[_numAttsOutput];											// Same as above
		Project* project = new Project(_schemaIn, _schemaOut, _numAttsInput, _numAttsOutput, _keepMe, j);		// ... = Final join operator
		*/
		if (_distinctAtts != 0)													// _distinctAtts != 0 -> DuplicateRemoval
		{
			// Create DuplicateRemoval
			/*
			DuplicateRemoval* duplicateRemoval = new DuplicateRemoval(_schemaIn, project);
			string outFile = "outfile";
			writeout = new WriteOut(_schemaOut, outFile, duplicateRemoval);			// Insert all relevant values into WriteOut
			// outFile is "outfile" because we are not using it yet
			*/
		}
		else
		{
			/*
			string outFile = "outfile";
			writeout = new WriteOut(_schemaOut, outFile, project);					// Insert all relevant values into WriteOut
																					// outFile is "outfile" because we are not using it yet
			*/
		}
		// Create the QueryExecutionTree
		/*
		_queryTree = writeout;
		*/
	}
	// free the memory occupied by the parse tree since it is not necessary anymore
}
