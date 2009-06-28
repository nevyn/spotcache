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
bind(int32_t i, const vector<uint8_t> &v) throw(Error)
{
	parent.check(sqlite3_bind_blob(stmt, i, &(*v.begin()), v.size(), SQLITE_TRANSIENT));
}

void
Sqlite::Statement::
bind(int32_t i, const string &v) throw(Error)
{
	parent.check(sqlite3_bind_text(stmt, i, v.c_str(), v.length(), SQLITE_TRANSIENT));
}

void
Sqlite::Statement::
bind(int32_t i, int32_t v) throw(Error)
{
	parent.check(sqlite3_bind_int(stmt, i, v));
}


int
Sqlite::Statement::
step(int32_t tryCount)
{
	int32_t status = sqlite3_step(stmt);
	lastStepStatus = status;
	
	if(status == SQLITE_BUSY && tryCount > 0) {
		sqlite3_sleep(100);
		return step(tryCount-1);
		
	}	else if (status == SQLITE_BUSY && tryCount == 0) {
		return SQLITE_BUSY;
		
	} else if (status == SQLITE_DONE || status == SQLITE_ROW) {
		return status;
	}
	
	// Any remaining cases are errors
	parent.check(status); // This will throw
	return SQLITE_ERROR; // never reached
}

bool
Sqlite::Statement::
hasData()
{
	return lastStepStatus == SQLITE_ROW;
}

int
Sqlite::Statement::
intColumn(int32_t i)
{
	return sqlite3_column_int(stmt, i);
}
double 
Sqlite::Statement::
doubleColumn(int32_t i)
{
	return sqlite3_column_double(stmt, i);
}
string
Sqlite::Statement::
textColumn(int32_t i)
{
	const char *text = (const char *)sqlite3_column_text(stmt, i);
	return string(text);
}

vector<uint8_t>
Sqlite::Statement::
blobColumn(int32_t i)
{
	vector<uint8_t> v(byteLength(i));
	column(i, v);
	return v;
}

void
Sqlite::Statement::
column(int32_t i, vector<uint8_t> &v)
{
	const uint8_t *data = (const uint8_t *)sqlite3_column_blob(stmt, i);
	int32_t datalen = sqlite3_column_bytes(stmt, i);
	if(v.size() < datalen)
		v.resize(datalen);
	std::copy(data, data+datalen, v.begin());
}

int
Sqlite::Statement::
byteLength(int32_t i)
{
	sqlite3_column_blob(stmt, i); // put sqlite in blob mode so next line works
	return sqlite3_column_bytes(stmt, i);
}



void
Sqlite::Statement::
reset() throw(Error)
{
	parent.check(sqlite3_reset(stmt));
	parent.check(sqlite3_clear_bindings(stmt));
}
