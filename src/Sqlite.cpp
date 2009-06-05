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

Sqlite::Statement::Ptr
Sqlite::
prepareFirst(const string &statement) {
	Sqlite::Statement::Ptr stmt(new Sqlite::Statement(*this));
	stmt->prepare(statement);
	return stmt;
}

void
Sqlite::
run(const string &statement)
{
	Sqlite::Statement::Ptr stmt = prepareFirst(statement);
	stmt->stepUntilNotBusy();
}




Sqlite::Statement::
Statement(Sqlite &parent_) : parent(parent_)
{
}

Sqlite::Statement::
~Statement()
{
	sqlite3_finalize(stmt);
}

string
Sqlite::Statement::
prepare(const string &statement)
{
	const char *rest = NULL;
	parent.check(sqlite3_prepare_v2(parent.db, statement.c_str(), -1, &stmt, &rest));
	return string(rest);
}

void
Sqlite::Statement::
bind(int i, int v)
{
	parent.check(sqlite3_bind_int(stmt, i, v));
}


void
Sqlite::Statement::
reset()
{
	parent.check(sqlite3_reset(stmt));
	parent.check(sqlite3_clear_bindings(stmt));
}

int
Sqlite::Statement::
step()
{
	int status = sqlite3_step(stmt);
	if(status == SQLITE_ERROR)
		parent.check(status); // This will throw
	return status;
}

int
Sqlite::Statement::
stepUntilNotBusy(int tryCount)
{
	int status = step();
	if(status == SQLITE_BUSY && tryCount > 0) {
		sqlite3_sleep(100);
		return stepUntilNotBusy(tryCount-1);
	}
	return status;
}

