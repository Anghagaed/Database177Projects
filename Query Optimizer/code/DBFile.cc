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

		ofstream outFile;

		outFile.open(f_path);

	}

}

int DBFile::Open (char* f_path) {

	inFile.open(f_path, std::ifstream::binary);	// operations performed in binary mode rather than text

	if (!inFile) {
		cout << "Error opening DBFile" << endl;
	}

}

void DBFile::Load (Schema& schema, char* textFile) {
	FILE* f = fopen(textFile, "r");						// Extract textFile to record
	Record r;
	int successP;
	Page p;									
	while(r.ExtractNextRecord(schema, *f) == 1)
	{
		successP = p.Append(r);							// Extract record to Page
		if (successP == 0)								// Insert old Page into File and create a new Page when the Page is empty
		{
			file.AddPage(p, file.GetLength() + 1);
			p.EmptyItOut();
			p.Append(r);
		}
	}
}

int DBFile::Close () {

	inFile.close();

}

void DBFile::MoveFirst () //issue here. Need to store the Record first some how.
{
	/*off_t curr_length = 0;//file.GetLength();
	Page pageTemp;						//page we are in
	file.GetPage(pageTemp, curr_length);
	pageTemp.myRecs.MoveToStart();*/
}

//appends record to end of file
void DBFile::AppendRecord (Record& rec) {
	Page p;
	file.GetPage(p, file.GetLength());	//get last page
	if (p.Append(rec)) {	//append record to the last page
		std::cout << "Appended Record\n";
	}
	else {	//if failed (page has not enough space) add a new page then append the record to the new page
		Page pg;
		file.AddPage(pg, file.GetLength());
		pg.Append(rec);
		std::cout << "Added a new page and appended record\n";
	}
}

int DBFile::GetNext (Record& rec) {

	Page page;
	Record firstOne;

	if (file.GetPage(page, 0) == true) {

		page.GetFirst(firstOne);
		rec = firstOne;

		return 1;
	}
	else {
		return 0;
	}

}
