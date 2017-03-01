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
	while (ptr != NULL) {
		ptr = ptr->next;
		++size;
	}
	return size;
}

void QueryOptimizer::pushDownSelection(AndList* _predicate) {
	pushDownList.clear();
	// create pushDown struct and push it to pushDownList
	
}
/*	Do greedy here
 * 	Implementation refers to optimization.algorithm
 *	This Greedy Algorithm will do the Pre-processing stage and then repeat stage 3 of pre-processing but treating the result as a single table
*/
void QueryOptimizer::greedy(TableList* _tables, AndList* _predicate, OptimizationTree* _root) {
	EfficientMap<KeyString, OptimizationTree> OptiMap;
	vector<string> currentKey;
	vector<int> popKey;
	int size = tableSize(_tables);
	
	pushDownSelection(_predicate);
	// Pre-processing step 1 : Filling Map with individual table
	TableList* ptrT = _tables;
	Schema* schem;
	while (ptrT != NULL) {
		unsigned int tTuples;
		string tName = ptrT->tableName;
		
		catalog->GetNoTuples(tName, tTuples);
		
		
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
	
	KeyString key;
	OptimizationTree* optimal, currentOptimal;
	int startingSize = currentKey.size();
	int J1, J2;
	unsigned int noTuples = UINT_MAX;
	for (int i = 0; i < startingSize; ++i) {
		key = KeyString(currentKey[i]);
		currentOptimal = OptiMap.Find(key);
		for (int j = i + 1; j < startingSize - 1; ++j) {
			key = KeyString(currentKey[j]);
			OptimizationTree* newOptimal = new OptimizationTree();
			OptimizationTree* right = new OptimizationTree();
			OptimizationTree* left = new OptimizationTree();
			
			newOptimal->CopyFrom(currentOptimal);
			left->CopyFrom(currentOptimal);
			right->CopyFrom(OptiMap.Find(key));
			
			// setting up the tree to be joined
			newOptimal->tables.push_back(right->tables[0]);
			newOptimal->tuples.push_back(right->tuples[0]);
			
			// compute cost using join
			
			// stores cost of this join in newOptimal->noTuples
			newOptimal->rightChild = right;
			newOptimal->leftChild = left;
			right->parent = newOptimal;
			left->parent = newOptimal;
			if (noTuples > newOptimal->noTuples) {
				optimal = newOptimal;
				noTuples = newOptimal->noTuples;
				J1 = i;
				J2 = j;
			}
			
			// Storing pointers to be deleted in destructor
			toBeDelete.push_back(right);
			toBeDelete.push_back(newOptimal);
			toBeDelete.push_back(left);
		}
	}
	popKey[J1] = 0;
	popKey[J2] = 0;
	_root = optimal;
	
	// Greedy repeating step 3 ad infinitum
	
	OptimizationTree* temp;
	for (int i = 0; i < size - 2; ++i) {
		// Reset noTuples
		noTuples = UINT_MAX;
		for (int j = 0; j < currentKey.size(); ++j) {
			if (popKey[j]) {
				key = KeyString(currentKey[j]);
				OptimizationTree* newOptimal = new OptimizationTree();
				OptimizationTree* right = new OptimizationTree();
				
				newOptimal->CopyFrom(*optimal);
				
				right->CopyFrom(OptiMap.Find(key));
				
				newOptimal->tables.push_back(right->tables[0]);
				newOptimal->tuples.push_back(right->tuples[0]);
				
				// compute join cost
				
				// stores cost of this join in newOptimal->noTuples
				if (noTuples > newOptimal->noTuples) {
					temp = newOptimal;
					noTuples = newOptimal->noTuples;
					temp->rightChild = right;
					right->parent = temp;
					J2 = j;
				}
				
				toBeDelete.push_back(right);
				toBeDelete.push_back(newOptimal);
			}
		}
		optimal->parent = temp;
		currentOptimal.leftChild = optimal;
	}
	
}
