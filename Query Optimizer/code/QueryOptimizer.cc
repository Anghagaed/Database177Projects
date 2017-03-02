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
OptimizationTree::OptimizationTree() {
	parent = NULL;
	leftChild = NULL;
	rightChild = NULL;
}
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
	//cout << "In Query Optimizer destructor" << endl;
	for (int i = 0; i < toBeDelete.size() - 1; ++i) {
		delete toBeDelete[i];
	}
	//cout << "Out of Query Optimizer Destructor" << endl;
}

OptimizationTree* QueryOptimizer::singleNode(string& tName, unsigned int & tTuples) {
	// compute some push down tuples size total after cost
	for (int i = 0; i < pushDownList.size(); ++i) {
		if (!tName.compare(pushDownList[i].tableName)) {
			if (pushDownList[i].code == 7) {
				unsigned int temp;
				catalog->GetNoDistinct(tName, pushDownList[i].attName, temp);
				tTuples /= temp;
			}
			else {
				tTuples /= 3;
			}
		}
	}

	OptimizationTree* _root = new OptimizationTree();
	_root->tables.push_back(tName);
	_root->tuples.push_back(tTuples);
	_root->noTuples = 0;
	toBeDelete.push_back(_root);

	return _root;
}
/*
 * How it looks in main: 	
 *	OptimizationTree ptr;
 *	optimizer.Optimize(tables, predicate, &ptr);
*/
void QueryOptimizer::Optimize(TableList* _tables, AndList* _predicate,
	OptimizationTree* _root) {
	//cout << _root << endl;
	//std::cout << "Starting Optimize" << std::endl;
	// compute the optimal join order
	OptimizationTree* tree;
	int size = tableSize(_tables);
	getPredicate(_predicate);
	//cout << _root << endl;
	if (size == 1) {
		unsigned int tTuples;
		string tName = _tables->tableName;

		catalog->GetNoTuples(tName, tTuples);
		tree = singleNode(tName, tTuples);
	}
	else if (size < 8) {
		tree = greedy(_tables, _predicate);
	}
	else {
		//tree = partition(_tables, _predicate);
	} 
	_root->Swap(*tree);
}

/*	
 *	Do greedy here
 * 	Implementation refers to optimization.algorithm
 *	This Greedy Algorithm will do the Pre-processing stage and then repeat stage 3 of pre-processing but treating the result as a single table
*/
OptimizationTree* QueryOptimizer::greedy(TableList* _tables, AndList* _predicate) {
	//std::cout << "Starting Greedy " << std::endl;
	EfficientMap<KeyString, OptimizationTree> OptiMap;
	OptimizationTree *_root;
	vector<string> currentKey;
	vector<int> popKey;
	int size = tableSize(_tables);
	
	// Pre-processing step 1 : Filling Map with individual table
	TableList* ptrT = _tables;
	Schema* schem;
	//cout << endl;
	//cout << "Starting 1st Preprocessing" << endl;
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
	
	//cout << "Ending 1st Preprocessing" << endl;
	// Preprocessing step 2 : Push-Down Selections
	//cout << "Starting 2nd Preprocessing" << endl;
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
	//cout << "Ending 2nd Preprocessing" << endl;
	
	// Preprocessing step 3 : Join for 2 tables
	//cout << "Starting 3rd Preprocessing" << endl;
	// Only work if table size if greater than 2
	if (size > 1) {
		KeyString key;
		OptimizationTree *optimal, *currentOptimal;
		int startingSize = currentKey.size();
		int J1, J2;
		unsigned int noTuples = UINT_MAX;
		for (int i = 0; i < startingSize; ++i) {
			key = KeyString(currentKey[i]);
			currentOptimal = &OptiMap.Find(key);
			for (int j = i + 1; j < startingSize; ++j) {
				key = KeyString(currentKey[j]);
				OptimizationTree* newOptimal = new OptimizationTree();
				OptimizationTree* right = new OptimizationTree();
				OptimizationTree* left = new OptimizationTree();

				newOptimal->CopyFrom(*currentOptimal);
				left->CopyFrom(*currentOptimal);
				right->CopyFrom(OptiMap.Find(key));

				newOptimal->tables.push_back(right->tables[0]);
				newOptimal->tuples.push_back(right->tuples[0]);

				string t1 = newOptimal->tables[0];
				string t2 = newOptimal->tables[1];

				for (int k = 0; k < joinList.size(); ++k) {

					string *j1 = &joinList[k].table1;
					string *j2 = &joinList[k].table2;
					unsigned int temp1, temp2;
					newOptimal->noTuples = newOptimal->tuples[0] * newOptimal->tuples[1];
					if ((!t1.compare(*j1) || !t1.compare(*j2)) && (!t2.compare(*j1) || !t2.compare(*j2))) {
						catalog->GetNoDistinct(*j1, joinList[k].att1, temp1);
						catalog->GetNoDistinct(*j2, joinList[k].att2, temp2);
						newOptimal->noTuples /= (temp1 > temp2) ? temp1 : temp2;
					}
				}

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
				toBeDelete.push_back(right);
				toBeDelete.push_back(newOptimal);
				toBeDelete.push_back(left);
			}
		}

		popKey[J1] = 0;
		popKey[J2] = 0;
		_root = optimal;
		//cout << "Best of 2 is: " << endl;
		//cout << _root->tables.size() << endl;
		//cout << _root->tables[0] << " " << _root->tables[1] << endl;
		//cout << _root->noTuples << endl;
		//cout << "Ending 3rd Preprocessing" << endl;

		// Greedy repeating step 3 ad infinitum
		//cout << "Starting Greedy Repeat" << endl;
		if (size > 2) {
			//cout << "Size - 2 is " << size - 2 << endl;
			//cout << "PopKey size is " << popKey.size() << endl;
			for (int i = 0; i < size - 2; ++i) {
				//cout << "Starting Outer Loop " << i << endl;
				noTuples = UINT_MAX;
				
				for (int j = 0; j < popKey.size(); ++j) {
					//cout << "Starting Inner Loop " << j << endl;
					if (popKey[j]) {
						//cout << "New creation" << endl;
						key = KeyString(currentKey[j]);
						OptimizationTree* newOptimal = new OptimizationTree();
						OptimizationTree* right = new OptimizationTree();

						newOptimal->CopyFrom(*_root);
						right->CopyFrom(OptiMap.Find(key));

						newOptimal->tables.push_back(right->tables[0]);
						newOptimal->tuples.push_back(right->tuples[0]);
						
						newOptimal->rightChild = right;
						right->parent = newOptimal;
						// compute cost
						//cout << "Computing cost" << endl;
						newOptimal->noTuples *= right->tuples[0];
						for (int a = 0; a < joinList.size(); ++a) {
							//cout << "joinList " << a << endl;
							string j1 = right->tables[0];
							if (!j1.compare(joinList[a].table1) || !j1.compare(joinList[a].table2)) {
								for (int b = 0; b < newOptimal->tables.size(); ++b) {
									string j2 = newOptimal->tables[b];
									if (!j2.compare(joinList[a].table1) || !j2.compare(joinList[a].table2)) {
										//cout << "before catalog" << endl;
										unsigned int temp1, temp2;
										catalog->GetNoDistinct(joinList[a].table1, joinList[a].att1, temp1);
										catalog->GetNoDistinct(joinList[a].table2, joinList[a].att2, temp2);
										//cout << "after catalog" << endl;
										//cout << newOptimal->noTuples << endl;
										newOptimal->noTuples /= (temp1 > temp2) ? temp1 : temp2;
										//cout << newOptimal->noTuples << endl;
										//cout << "after calculation" << endl;
						}	}	}	}
						//cout << "Determining if lowest cost" << endl;
						//cout << noTuples << endl;
						//cout << newOptimal->noTuples << endl;
						if (noTuples > newOptimal->noTuples) {
							currentOptimal = newOptimal;
							noTuples = newOptimal->noTuples;
							J1 = j;
						}

						toBeDelete.push_back(newOptimal);
						toBeDelete.push_back(right);
					}
					//cout << "Ending inner loop" << j << endl;
				}
				popKey[J1] = 0;
				_root->parent = currentOptimal;
				currentOptimal->leftChild = _root;
				_root = currentOptimal;
				//cout << "Ending outer loop" << i << endl;
			}
		}
		//cout << "Ending Greedying Repeat" << endl;
	}

	//cout << "Ending Greedy" << endl;
	//cout << endl<< _root->noTuples << endl;
	//cout << _root << endl;
	return _root;
}
void QueryOptimizer::partition(TableList* _tables, AndList* _predicate) {
	cout << "I am out " << endl;
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
