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
	cout << "before catalog" << endl;
	delete catalog;
	cout << "after catalog" << endl;
	delete optimizer;
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

//Recursive function to traverse the optimization tree and create join operators along the way
//traverses in post order - reaches both children before parent
//returns the root of the join tree as a relational op pointer
//resulting schema can be accessed with the root's schemaOut
RelationalOp * QueryCompiler::JoinTree(OptimizationTree * root, AndList * _predicate) {
	RelationalOp* left = JoinTree(root->leftChild, _predicate);	//get the relational op of left child	
	RelationalOp* right = JoinTree(root->rightChild, _predicate);	//get the relational op of right child
	RelationalOp *result;
	
	//check how many tables are at this node
	if (root->tables.size() < 2) {	//if it's not 2 or more, then search through our scan/select maps to find the relational op
		return GetRelOp(root->tables[0]);
	}

	//if there are 2 or more tables, then we need to join
	// gets the left/right schemas/relationalOp from the children of this node
	else {
		//make the join operator

		//get left schema somehow
		//downcast left into a scan/select/join and get the schema/schemaOut???
		//get the type and then downcast accordingly???
		Schema left_schema;

		//save left schema
		Schema left_temp = left_schema;

		//get right schema somehow
		//downcast left into a scan/select/join and get the schema/schemaOut???
		Schema right_schema;

		//get the predicate
		CNF predicate;
		//predicate.ExtractCNF(_predicate, left_schema, right_schema);

		//get resulting schema
		//left_schema.Append(right_schema);	//leftschema now holds the resulting schema of join

		//Join *j = new Join(left_temp, right_schema, left_schema, predicate, left, right);
		//return j;	//return our relational op
	}
}

void QueryCompiler::Compile(TableList* _tables, NameList* _attsToSelect,
	FuncOperator* _finalFunction, AndList* _predicate,
	NameList* _groupingAtts, int& _distinctAtts,
	QueryExecutionTree& _queryTree) {
	
	// create a SCAN operator for each table in the query
	int size = tableSize(_tables);
	for (int i = 0; i < size; i++)
	{
		Schema mySchema;
		string temp = _tables->tableName;
		//string temp = iterator->tableName;
		catalog->GetSchema(temp, mySchema);
		DBFile myFile = DBFile();
		string pathConvert = "catalog.txt";//going to be converted to char *
		char* path = new char[pathConvert.length() + 1];
		strcpy(path, pathConvert.c_str());
		myFile.Open(path);
		Scan *myScan = new Scan(mySchema, myFile, temp);
		ScanMap.push_back(*myScan);
		_tables = _tables->next;
		// push-down selections: create a SELECT operator wherever necessary
		Record recTemp;
		CNF cnfTemp;
		cnfTemp.ExtractCNF(*_predicate, myScan->getSchema(), recTemp);
		if (cnfTemp.ExtractCNF(*_predicate, myScan->getSchema(), recTemp) == 0)// builds CNF and Record needed. Now we have Schema, Record, and CNF. Just need RelationOp
		{
			Select *mySelect = new Select(myScan->getSchema(), cnfTemp, recTemp, myScan, temp);
			SelectMap.push_back(*mySelect);
		}
	}
	/*
	cout<<SelectMap.Length();

	// call the optimizer to compute the join order
	OptimizationTree* root;
	optimizer->Optimize(_tables, _predicate, root);
	
	// create join operators based on the optimal order computed by the optimizer
	Join* j;	//j will point to root of join tree
	if (size > 8) {
		// left-deep trees

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
		//j = JoinTree(root, _predicate);	//need to cast relationalop to join somehow
	}

	// create the remaining operators based on the query

	// connect everything in the query execution tree and return
	// Create WriteOut here
	/*
	WriteOut* writeout;
	*/
	// ASSUME: Final Join's Schema = Jschema; Final Join's Pointer = root
	if (_groupingAtts != 0)														// Non-empty _groupingAtts -> GroupBy
	{
		// Create GroupBy here
		/*
		Schema _schemaIn = Jschema;												// Set it equal to join's final schema
		Schema _schemaOut;														// Set it equal to nothing (not really important at the moment)
		vector<Attribute> att = _schemaIn.GetAtts();							// Get the Attributes
		int _atts_no = att.size();												// Get the Attribute size
		int _atts[_atts_no];													// Create an array of indices of Attributes
		for(int i = 0; i < att.size(); i++)
			_atts[i] = _schemaIn.Index(att[i].name);							// Receive indices of all Attributes in the Schema
		OrderMaker _groupingAtts = OrderMaker(_schemaIn, _atts, _atts_no);		// Insert all relevant values into OrderMaker
		Function _compute;
		_compute.GrowFromParseTree (_finalFunction, _schemaIn);					// Insert all relevant values into Function
		GroupBy* groupby = new GroupBy(_schemaIn, _schemaOut, _groupingAtts, _compute,	root);		// root = Final join operator
		string outFile = "outfile";
		writeout = new WriteOut(_schemaOut, outFile, groupby);						// Insert all relevant values into WriteOut
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
