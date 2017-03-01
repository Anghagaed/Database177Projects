#include <string>
#include <vector>
#include <iostream>
#include <climits>

#include "Schema.h"
#include "Comparison.h"
#include "QueryOptimizer.h"
#include "Comparison.h"
#include "EfficientMap.h"
#include "EfficientMap.cc"
#include "Keyify.h"
#include "Keyify.cc"

using namespace std;

void OptimizationTree::Swap(OptimizationTree& _withMe) {
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

void OptimizationTree::CopyFrom(OptimizationTree& _withMe) {
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
	cout << "Out of Query Optimizer Deconstructor" << endl;
}

void QueryOptimizer::Optimize(TableList* _tables, AndList* _predicate,
	OptimizationTree* _root) {
	std::cout << "Starting Optimize" << std::endl;
	// compute the optimal join order
	int size = tableSize(_tables);
	std::cout << "Size is: " << std::endl;
	getPredicate(_predicate);
	greedy(_tables, _predicate, _root);
	/*
	if (size > 8) {
		greedy(_tables, _predicate, _root);
	} else {
		partition(_tables, _predicate, _root);
	}
	*/
}

/*	Do greedy here
 * 	Implementation refers to optimization.algorithm
 *	This Greedy Algorithm will do the Pre-processing stage and then repeat stage 3 of pre-processing but treating the result as a single table
*/
void QueryOptimizer::greedy(TableList* _tables, AndList* _predicate, OptimizationTree* _root) {
	std::cout << "Starting Greedy " << std::endl;
	EfficientMap<KeyString, OptimizationTree> OptiMap;

	vector<string> currentKey;
	vector<int> popKey;
	int size = tableSize(_tables);
	
	// Pre-processing step 1 : Filling Map with individual table
	TableList* ptrT = _tables;
	Schema* schem;
	cout << endl;
	cout << "Starting 1st Preprocessing" << endl;
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
		ptrT = ptrT->next;
	}
	
	cout << "Ending 1st Preprocessing" << endl;
	
	for (int i = 0; i < currentKey.size(); ++i) {
		KeyString key = KeyString(currentKey[i]);
		OptimizationTree* ptr = &OptiMap.Find(key);
		cout << ptr->tables[0] << " " << ptr->tuples[0] << endl;
	}
	// Preprocessing step 2 : Push-Down Selections
	cout << endl;
	cout << "Starting 2nd Preprocessing" << endl;
	for (int i = 0; i < pushDownList.size(); ++i) {
		KeyString key = KeyString(pushDownList[i].tableName);
		OptimizationTree* ptr = &OptiMap.Find(key);
		//cout << pushDownList[i].tableName << " " << pushDownList[i].code << " " << pushDownList[i].attName << endl;
		if (pushDownList[i].code == 7) {
			unsigned int _noDistinct = 1;
			catalog->GetNoDistinct(pushDownList[i].tableName, pushDownList[i].attName, _noDistinct);
			//cout << "Distinct: " << _noDistinct << endl;
			ptr->tuples[0] /= _noDistinct;
		} else {
			ptr->tuples[0] /= 3;
		}
		//cout << ptr->tables[0] << " " << ptr->tuples[0] << endl;
	}
	cout << "Ending 2nd Preprocessing" << endl;
	
	// Preprocessing step 3 : Join for 2 tables
	cout << endl;
	cout << "Starting 3rd Preprocessing" << endl;
	cout << joinList.size() << endl;
	for (int w = 0; w < joinList.size(); ++w) {
		cout << joinList[w].table1 << endl;
		cout << joinList[w].att1 << endl;
		cout << joinList[w].table2 << endl;
		cout << joinList[w].att2 << endl;
		cout << endl;
	}
	KeyString key;
	OptimizationTree *optimal, currentOptimal;
	int startingSize = currentKey.size();
	//cout << "Starting Size is " << startingSize << endl;
	int J1, J2;
	unsigned int noTuples = UINT_MAX;
	for (int i = 0; i < startingSize; ++i) {
		key = KeyString(currentKey[i]);
		currentOptimal = OptiMap.Find(key);
		for (int j = i + 1; j < startingSize; ++j) {
			cout << "i is currently" << i << " j is currently " << j << endl;
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
			int size = newOptimal->tables.size();
			//cout << "Size is: " << size << " i is: " << i << " j is: " << j << endl;
			for (int w = 0; w < size; ++w) {
				//cout << newOptimal->tables[w] << " ";
			}
			cout << endl;
			// compute cost using join
			string t1 = newOptimal->tables[0];
			string t2 = newOptimal->tables[1];
			/*
			cout << "t1 is: " << t1 << endl;
			cout << "t2 is: " << t2 << endl;
			cout << endl;
			*/
			
			for (int k = 0; k < joinList.size(); ++k) {

				string *j1 = &joinList[k].table1;
				string *j2 = &joinList[k].table2;
				unsigned int temp1, temp2;
				newOptimal->noTuples = newOptimal->tuples[0] * newOptimal->tuples[1];
				if ((!t1.compare(*j1) || !t1.compare(*j2)) && (!t2.compare(*j1) || !t2.compare(*j2))) {
					//cout << t1 << " " << t2 << endl;
					//cout << *j1 << " " << *j2 << endl;
					//cout << endl;
					catalog->GetNoDistinct(*j1, joinList[k].att1, temp1);
					catalog->GetNoDistinct(*j2, joinList[k].att2, temp2);
					//cout << "D1: " << temp1 << " D2: " << temp2 << endl;
					newOptimal->noTuples /= (temp1 > temp2) ? temp1 : temp2;
				}
			}
			
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
	cout << "Best of 2 is: " << endl;
	cout << optimal->tables[0] << " " << optimal->tables[1] << endl;
	cout << optimal->noTuples << endl;
	_root = optimal;
	cout << "Ending 3rd Preprocessing" << endl;
	// Greedy repeating step 3 ad infinitum
	
	OptimizationTree* temp;
	cout << "Size - 2 is " << size - 2 << endl;
	for (int i = 0; i < size - 2; ++i) {
		// Reset noTuples
		for (int k = 0; k < popKey.size(); ++k) {
			cout << popKey[k] << " ";
		}
		cout << endl;
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
		popKey[J2] = 0;
	}
	
}
void QueryOptimizer::partition(TableList* _tables, AndList* _predicate, OptimizationTree* _root) {

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

void QueryOptimizer::getPredicate(AndList* _predicate) {
	pushDownList.clear();
	joinList.clear();

	AndList* myPredicate = _predicate;

	while (myPredicate != NULL) {

		ComparisonOp* myComp = myPredicate->left;

		if (myComp != NULL) {

			if (myComp->left->code == 3 && myComp->right->code != 3) {

				pushDown myPushObj;

				myPushObj.code = myComp->code;

				myPushObj.attName = myComp->left->value;

				string temp = myComp->left->value;

				myPushObj.tableName = findTableName(temp);

				pushDownList.push_back(myPushObj);

			}

			else if (myComp->left->code != 3 && myComp->right->code == 3) {

				pushDown myPushObj;

				myPushObj.code = myComp->code;

				myPushObj.attName = myComp->right->value;

				string temp = myComp->right->value;

				myPushObj.tableName = findTableName(temp);

				pushDownList.push_back(myPushObj);

			}

			else if (myComp->left->code == 3 && myComp->right->code == 3) {

				joinStuff myJoinObj;

				myJoinObj.att1 = myComp->left->value;
				myJoinObj.att2 = myComp->right->value;

				string temp = myComp->left->value;

				myJoinObj.table1 = findTableName(temp);

				temp = myComp->right->value;

				myJoinObj.table2 = findTableName(temp);

				joinList.push_back(myJoinObj);

			}


		}

		myPredicate = myPredicate->rightAnd;

	}

	// --------------------PRINTING LISTS-------------------
	/*

	for (int i = 0; i < pushDownList.size(); i++) {

	cout << pushDownList[i].attName;

	if (pushDownList[i].code == 5)
	cout << " < ";
	else if (pushDownList[i].code == 6)
	cout << " > ";
	else if (pushDownList[i].code == 7)
	cout << " = ";

	cout << "Some Constant" << endl;
	cout << pushDownList[i].tableName << endl;

	}

	for (int i = 0; i < joinList.size(); i++) {

	cout << joinList[i].att1 << " = " << joinList[i].att2 << endl;
	cout << joinList[i].table1 << " and " << joinList[i].table2 << endl;

	}
	*/

}

string QueryOptimizer::findTableName(string& attName) {

	for (int i = 0; i < catalog->currentTables.size(); i++) {

		vector<string> myAtts;

		catalog->GetAttributes(catalog->currentTables[i], myAtts);

		for (int j = 0; j < myAtts.size(); j++) {

			if (myAtts[j] == attName) {

				return catalog->currentTables[i];

			}

		}

		myAtts.clear();

	}

}
