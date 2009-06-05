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
check(int err) {
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



Sqlite::Statement::
Statement(Sqlite &parent_) : parent(parent_)
{
}
Statement::
~Statement()
{
	sqlite3_finalize(stmt);
}

void
Sqlite::Statement::
prepare(const string &statement)
{
	parent.check(sqlite3_prepare_v2(parent.db, statement.c_str(), -1, &stmt, NULL));
}