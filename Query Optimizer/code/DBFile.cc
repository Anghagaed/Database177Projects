#include <string>

#include "Config.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "DBFile.h"
#include "RelOp.h"
#include "Comparison.h"

using namespace std;


DBFile::DBFile () : fileName("") {
}

DBFile::~DBFile () {
}

DBFile::DBFile(const DBFile& _copyMe) :
	file(_copyMe.file),	fileName(_copyMe.fileName) {}

DBFile& DBFile::operator=(const DBFile& _copyMe) {
	// handle self-assignment first
	if (this == &_copyMe) return *this;

	file = _copyMe.file;
	fileName = _copyMe.fileName;

	return *this;
}

int DBFile::Create (char* f_path, FileType f_type) {

	if (f_type == Heap) {
		// create heap file


	}

}

int DBFile::Open (char* f_path) {

	inFile.open(f_path, std::ifstream::binary);	// operations performed in binary mode rather than text

	if (!inFile) {
		cout << "Error opening DBFile" << endl;
	}
}

void DBFile::Load (Schema& schema, char* textFile) {
	// Extract textFile to record
	FILE* f = fopen(textFile, "r");
	Record r;
	int success = r.ExtractNextRecord(schema, *f);
	if (success == 0)	// Error checking
	{
		std::cout << "Error -- Failed to extract record.\n";
		return;
	}
	// Insert record into page
	Page p;
	success = p.Append(r);
	if (success == 0)	// Error checking
	{
		std::cout << "Error -- Failed to append record.\n";
		return;
	}
	// Insert page into file
	file.AddPage(p, file.GetLength() + 1);
}

int DBFile::Close () {
/*
	inFile.close();*/
}

void DBFile::MoveFirst () {

}

void DBFile::AppendRecord (Record& rec) {
}

int DBFile::GetNext (Record& rec) {

	// Selection

		//Comparison->run();


}
