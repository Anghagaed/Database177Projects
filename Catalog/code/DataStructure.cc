# include "DataStructure.h"

using namespace std;

tableInfo::tableInfo() {
	name = "";
	path = "";
	nuTuples = 0;
}

tableInfo::tableInfo(string na, string pa, int tu) { 
	name = na;
	path = pa;
	nuTuples = tu;
};

void tableInfo::Swap(tableInfo& withMe) {
	SWAP(name, withMe.name);
	SWAP(path, withMe.path);
	SWAP(nuTuples, withMe.nuTuples);
}

void tableInfo::CopyFrom(tableInfo& withMe) {
	this->name = withMe.name;
	this->path = withMe.path;
	this->nuTuples = withMe.nuTuples;
}

void tableInfo::setName(string na) {
	name = na;
}

void tableInfo::setPath(string pa) {
	path = pa;
}

void tableInfo::setTuples(int tu) {
	nuTuples = tu;
}

string tableInfo::getName() {
	return name;
}

string tableInfo::getPath() {
	return path;
}

int tableInfo::getTuples() {
	return nuTuples;
}

attsInfo::attsInfo() {
	name = "";
	disVal = 0;
}

attsInfo::attsInfo(string na, Type ty, int di) { 
	name = na;
	type = ty;
	disVal = di;
};

void attsInfo::Swap(attsInfo& withMe) {
	SWAP(name, withMe.name);
	SWAP(type, withMe.type);
	SWAP(disVal, withMe.disVal);
}

void attsInfo::CopyFrom(attsInfo& withMe) {
	this->name = withMe.name;
	this->type = withMe.type;
	this->disVal = withMe.disVal;
}

void attsInfo::setName(string na) {
	name = na;
}

void attsInfo::setType(Type ty) {
	type = ty;
}

void attsInfo::setDistinct(int di) {
	disVal = di;
}

string attsInfo::getName() {
	return name;
}

Type attsInfo::getType() {
	return type;
}

int attsInfo::getDistinct() {
	return disVal;
}
