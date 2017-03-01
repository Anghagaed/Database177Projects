#ifndef _QUERY_OPTIMIZER_H
#define _QUERY_OPTIMIZER_H

#include "Schema.h"
#include "Catalog.h"
#include "ParseTree.h"
#include "RelOp.h"
#include "EfficientMap.h"
#include "InefficientMap.h"
#include "Keyify.h" 

#include <string>
#include <vector>

using namespace std;


// data structure used by the optimizer to compute join ordering
class OptimizationTree {
	public:
	// list of tables joined up to this node
	vector<string> tables;
	// number of tuples in each of the tables (after selection predicates)
	vector<int> tuples;
	// number of tuples at this node
	int noTuples;

	// connections to children and parent
	OptimizationTree* parent;
	OptimizationTree* leftChild;
	OptimizationTree* rightChild;
	
	// Swap and CopyFrom for EfficientMap
	void Swap(OptimizationTree& _withMe);
	void CopyFrom(OptimizationTree& _withMe);
};

struct pushDown {
	string tableName;
	int code;				// 0 >
							// 1 < 
							// 2 =
	string attName;				
};

struct joinStuff {
	string table1;
	string att1;
	string table2;
	string att2;
};

class QueryOptimizer {
private:
	Catalog* catalog;
	vector<OptimizationTree*> toBeDelete;
	
	vector<pushDown>  pushDownList;
	vector<joinStuff> joinList;
	//EfficientMap<KeyString, OptimizationTree> OptiMap;
private:
	int tableSize(TableList* _tables);
	void greedy(TableList* _tables, AndList* _predicate, OptimizationTree* _root);
	void partition(TableList* _tables, AndList* _predicate, OptimizationTree* _root);
	void makingJoin(AndList* _predicate);
	string findTableName(string& attName);
public:
	QueryOptimizer(Catalog& _catalog);
	virtual ~QueryOptimizer();
	void pushDownSelection(AndList* _predicate);
	void Optimize(TableList* _tables, AndList* _predicate, OptimizationTree* _root);
};

#endif // _QUERY_OPTIMIZER_H
