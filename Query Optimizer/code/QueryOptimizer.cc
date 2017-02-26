#include <string>
#include <vector>
#include <iostream>
#include <climits>

#include "Schema.h"
#include "Comparison.h"
#include "QueryOptimizer.h"
#include "Comparison.h"

using namespace std;

void OptimizationTree::Swap(OptimizationTree _withMe) {
	// Swap vector
	this->tables.swap(_withMe.tables);
	this->tuples.swap(_withMe.tuples);
	
	// Swap noTuples
	int tempI = noTuples;
	this->noTuples = _withMe.noTuples;
	_withMe.noTuples = tempI;
	
	// Swap Pointer
	OptimizationTree* temp;
	temp = this->parent;
	this->parent = _withMe.parent;
	_withMe.parent = temp;
	temp = this->leftChild;
	this->leftChild = _withMe.leftChild;
	_withMe.leftChild = temp;
	temp = this->rightChild;
	this->rightChild = _withMe.rightChild;
	_withMe.rightChild = temp;
}

void OptimizationTree::CopyFrom(OptimizationTree _withMe) {
	this->tables = _withMe.tables;
	this->tuples = _withMe.tuples;
	this->noTuples = _withMe.noTuples;
	
	this->parent = _withMe.parent;
	this->leftChild = _withMe.leftChild;
	this->rightChild = _withMe.rightChild;
}

QueryOptimizer::QueryOptimizer(Catalog& _catalog) : catalog(&_catalog) {
}

QueryOptimizer::~QueryOptimizer() {
	for (int i = 0; i < toBeDelete.size() - 1; ++i) {
		delete toBeDelete[i];
	}
}

void QueryOptimizer::Optimize(TableList* _tables, AndList* _predicate,
	OptimizationTree* _root) {
	// compute the optimal join order
	int size = tableSize(_tables);
	
	if (size > 8) {
		greedy(_tables, _predicate, _root);
	} else {
		partition(_tables, _predicate, _root);
	}
}

int QueryOptimizer::tableSize(TableList* _tables) {
	int size = 0;
	TableList* ptr = _tables;
	while (ptr != null) {
		ptr = ptr.next;
		++size;
	}
	return size;
}
/*	Do greedy here
 * 	Implementation refers to optimization.algorithm
 *	This Greedy Algorithm will do the Pre-processing stage and then repeat stage 3 of pre-processing but treating the result as a single table
*/
void QueryOptimizer::greedy(TableList* _tables, AndList* _predicate, OptimizationTree* _root) {
	EfficientMap<KeyString, OptimizationTree> OptiMap;
	OptimizationTree* optimal;
	vector<string> currentKey;
	vector<int> popKey;
	int size = tableSize(_tables);
	
	// Pre-processing step 1 : Filling Map with individual table
	TableList* ptrT = _tables;
	Schema* schem;
	while (ptrT != null) {
		unsigned int tTuples;
		string tName = ptrT->tableName;
		
		catalog.GetNoTuples(tName, tTuples);
		
		
		KeyString key(tName);
		currentKey.push_back(tName);
		popKey.push_back(1);
		OptimizationTree toPush;
		
		toPush.tables.push_back(tName);
		toPush.tuples.push_back(tTuples);
		toPush.noTuples = 0;
		
		OptiMap.Insert(key, toPush);
	}
	
	// Preprocessing step 2 : Push-Down Selections
	
	// Preprocessing step 3 : Join for 2 tables
	// Greedy repeating step 3 ad infinitum
	KeyString key;
	key = KeyString(currentKey[0]);
	optimal = OptiMap.Find(key);
	optimal.noTuples = UINT_MAX;
	
	for (int i = 0; i < size - 1; ++i) {
		int startingSize = AvailKey.size() - 2;
		OptimizationTree* bestOptimal;
		int noTuples1 = UINT_MAX;
		for (int i = 1; i < startingSize; ++i) {
			string joinT1 = currentKey[i];
			OptimizationTree* currentOptimal;
			int noTuples2 = UINT_MAX;
			for (int j = i + 1; j < startingSize + 1; ++j) {
				if (popKey[j]) {
					string joinT2 = currentKey[j];
					OptimizationTree* newOptimal = new OptimizationTree();						// Yun tells me to do this
					newOptimal->CopyFrom(optimal);
					key = KeyString(currentKey[j]);
					OptimizationTree* toPush = new OptimizationTree();
					toPush.CopyFrom(OptiMap.Find(key));
					
					newOptimal->tables.push_back(toPush->tables[0]);
					newOptimal->tuples.push_back(toPush->tuples[0]);
					
					// Compute cost using join 
					
					// Memory Leak management
					// Possible bug, needs to triple check
					// if I can do this
					toBeDelete.push_back(newOptimal);
					toBeDelete.push_back(toPush);
					
					newOptimal->rightChild = toPush;
					toPush->parent = newOptimal;
					if (noTuples2 > newOptimal->noTuples) {
						currentOptimal = newOptimal;
						noTuples2 = newOptimal->noTuples;
					}
					
					popKey[j] = 0;
				}
			}
			if (noTuples1 > currentOptimal->noTuples) {
				noTuples1 = currentOptimal->noTuples;
				bestOptimal = currentOptimal;
			}
		}
		optimal->parent = bestOptimal;
		bestOptimal->leftChild = optimal;
		optimal = bestOptimal;
	}
}


























