#ifndef _PARSE_TREE_
#define _PARSE_TREE_

// type of operands that can appear in a CNF expression
#define FLOAT 1
#define INTEGER 2
#define NAME 3
#define STRING 4

// type of operators that can appear in a CNF expression
#define LESS_THAN 5
#define GREATER_THAN 6
#define EQUALS 7


// functional expressions
struct FuncOperand {
	// type of the operand
	int code;
	// actual operand
	char* value;
};

struct FuncOperator {
	// type of operator
	int code;
	// operators on the left and on the right
	struct FuncOperator* leftOperator;
	struct FuncOperand* leftOperand;
	struct FuncOperator* right;
};

struct TableList {
	// table name
	char* tableName;
	// next table
	struct TableList* next;
};

struct NameList {
	// name
	char* name;
	// next name in the list
	struct NameList* next;
};

struct Operand {
	// type of the operand
	int code;
	// actual operand
	char* value;
};

struct ComparisonOp {
	// operator type
	int code;
	// operands on the left and on the right
	struct Operand* left;
	struct Operand* right;
};

struct AndList {
	// disjunction to the left of the AND
	struct ComparisonOp* left;
	// AndList to the right of the AND
	struct AndList* rightAnd;
};

/* Data Structure to hold a name string for the new grammar outlined in newGrammar.alg
 * Code let us know what category the name belongs to
 * 0	Attribute Name
 * 1	Attribute Type
 * 2	Table
 * 3	Index
 * 4	File Name	
 */
struct GenericName {
	// category that the name belongs to
	int code;
	// name
	char* name;
	// Not sure if needed? May need it for Parser?
	GenericName* next;
};

// Not sure if needed if GenericName has a Next pointer
struct AttsList {
	struct GenericName* name;
	struct GenericName* type;
	struct AttsList* next;
};

/*
struct AttributeName {
	char* attName;
};

struct TypeName {
	char* typeName;
};

struct TableName {
	char* tableName;
};

struct TextFile {
	char* textFile;
};

struct IndexName {
	char* indexName;
};
*/
#endif
