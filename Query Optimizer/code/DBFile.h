#ifndef DBFILE_H
#define DBFILE_H

#include <string>

#include "Config.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include <fstream>

using namespace std;


class DBFile {
private:
	File file;
	string fileName;
	Page temp;
	int count;
	ifstream inFile;
	Page p;

public:
	DBFile ();
	virtual ~DBFile ();
	DBFile(const DBFile& _copyMe);
	DBFile& operator=(const DBFile& _copyMe);

	int Create (char* fpath, FileType file_type);
	int Open (char* fpath);
	int Close ();

	void Load (Schema& _schema, char* textFile);

	void MoveFirst ();
	void AppendRecord (Record& _addMe);
	void AppendLast();
	int GetNext (Record& _fetchMe);
};

#endif //DBFILE_H
