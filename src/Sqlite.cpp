/*
 *  Sqlite.cpp
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-06-04.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */

#include "Sqlite.h"

void
Sqlite::
check(int32_t err) throw(Error)
{
	if(err != SQLITE_OK) {
		Sqlite::Error exc = Sqlite::Error(sqlite3_errmsg(db));
		exc.errorCode = err;
		throw exc;
	}		
}


Sqlite::
Sqlite(const string &path)
{
	try {
		check(sqlite3_open(path.c_str(), &db));
	} catch (Error &e) {
		sqlite3_close(db);
		throw;
	}
}
Sqlite::
~Sqlite()
{
	sqlite3_close(db);
}

Sqlite::Statement::Ptr
Sqlite::
prepareFirst(const string &statement) {
	Sqlite::Statement::Ptr stmt(new Sqlite::Statement(*this));
	stmt->prepare(statement);
	return stmt;
}

Sqlite::Blob::Ptr
Sqlite::
blob(const string &table, const string &columnName, int64_t rowid)
{
	Sqlite::Blob::Ptr blob(new Sqlite::Blob(*this, table, columnName, rowid));
	return blob;
}

void
Sqlite::
run(const string &statement) throw(Error)
{
	Sqlite::Statement::Ptr stmt = prepareFirst(statement);
	int32_t status = stmt->step();
	if(status == SQLITE_BUSY) {
		Sqlite::Error exc = Sqlite::Error("Database was busy and unable to perform request "+statement);
		exc.errorCode = SQLITE_BUSY;
		throw exc;		
	}
}

