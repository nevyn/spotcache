/*
 *  Sqlite+Statement.cpp
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-07-06.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */

#include "Sqlite.h"

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

void
Sqlite::Statement::
bind64(int32_t i, int64_t v) throw(Error)
{
	parent.check(sqlite3_bind_int64(stmt, i, v));
}

void
Sqlite::Statement::
bindZeroBlob(int32_t i, int32_t n) throw(Error)
{
	parent.check(sqlite3_bind_zeroblob(stmt, i, n));
}



int32_t
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

int32_t
Sqlite::Statement::
intColumn(int32_t i)
{
	return sqlite3_column_int(stmt, i);
}
int64_t 
Sqlite::Statement::
int64Column(int32_t i)
{
	return sqlite3_column_int64(stmt, i);
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

int32_t
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
