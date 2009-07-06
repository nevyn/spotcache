/*
 *  Sqlite+Blob.cpp
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-07-06.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */

#include "Sqlite.h"

Sqlite::Blob::
Blob(Sqlite &parent_,
		 const string &table,
		 const string &columnName,
		 int64_t rowid) : parent(parent_)
{
	int status = sqlite3_blob_open(parent.db,
																 NULL,					     // dbname
																 table.c_str(),     // tablename
																 columnName.c_str(),
																 rowid,
																 true,							 // writable?
																 &blob);
	parent.check(status);
}
Sqlite::Blob::
~Blob()
{
	sqlite3_blob_close(blob);
}

int32_t
Sqlite::Blob::
size()
{
	return sqlite3_blob_bytes(blob);
}

vector<uint8_t>
Sqlite::Blob::
read(int32_t bytesToRead,
		 int32_t offset) throw(Error)
{
	if(bytesToRead == -1) bytesToRead = size();
	
	vector<uint8_t> dest(bytesToRead);
	read(dest.begin(), bytesToRead, offset);
	return dest;
}

void
Sqlite::Blob::
read(vector<uint8_t>::iterator destination, 
					int32_t bytesToRead,
					int32_t offset) throw(Error)
{
	if(bytesToRead == -1) bytesToRead = size();
	
	parent.check(sqlite3_blob_read(blob, &(*destination), bytesToRead, offset));
}

void
Sqlite::Blob::
write(const vector<uint8_t> &source, int32_t offset) throw(Error)
{
	write(source.begin(), source.end(), offset);
}
void
Sqlite::Blob::
write(vector<uint8_t>::const_iterator from,
			vector<uint8_t>::const_iterator to,
			int32_t destOffset) throw(Error)
{
	parent.check(sqlite3_blob_write(blob, &(*from), to-from, destOffset));
}

