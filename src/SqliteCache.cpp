/*
 *  SqliteCache.cpp
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-06-03.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */

#include "SqliteCache.h"
#include <openssl/md5.h>
#include <algorithm>

/// Schema
static string schema = 
"create table if not exists cache ( "
"  object_id varchar(16) unique primary key, "
"  data blob not null default \"\", "
"  completed bool not null default false"
");";

SqliteCache::
SqliteCache(const string &path, const vector<uint8_t> &encryption_key)
: db(path), key(encryption_key)
{
	db.run(schema);
	hasStmt = db.prepareFirst("select completed from cache where object_id = ?;");
	readStmt = db.prepareFirst("select data from cache where object_id = ?;");
	writeStmt = db.prepareFirst("insert into cache (object_id, data, completed) values (?, ?, ?);");
	removeStmt = db.prepareFirst("delete from cache where object_id = ?;");
}

SqliteCache::
~SqliteCache()
{
}

bool
SqliteCache::
hasObject(const ObjectId &obj_id)
{
	return objectIsAvailable(obj_id) == ObjectCached;
}

Cache::CacheAvailability
SqliteCache::
objectIsAvailable(const ObjectId &obj_id)
{
	Sqlite::Statement::Resetter resetHas(*hasStmt);
	
	hasStmt->bind(1, keyify(obj_id));
	hasStmt->step();
	
	if(!hasStmt->hasData())
		return ObjectNotCached;
	else if(hasStmt->intColumn(0) == false)
		return ObjectPartiallyCached;
	else
		return ObjectCached;
}

bool
SqliteCache::
readObject(const ObjectId &obj_id, vector<uint8_t> &result)
{
	Sqlite::Statement::Resetter resetRead(*readStmt);
	
	readStmt->bind(1, keyify(obj_id));

	readStmt->step();
	
	if(readStmt->hasData()) {
		readStmt->column(0, result);
		return true;
	}
	return false;
}

bool
SqliteCache::
writeObject(const ObjectId &obj_id, 
						const vector<uint8_t>	 &value,
						bool completesInsertion) throw(AppendingToCompletedObjectException)
{
	CacheAvailability ca = objectIsAvailable(obj_id);
	
	if(ca == ObjectCached)
		 throw AppendingToCompletedObjectException();
	
	string lookupKey = keyify(obj_id);
	
	if(ca == ObjectNotCached) {
		Sqlite::Statement::Resetter resetWrite(*writeStmt);
		writeStmt->bind(1, lookupKey);
		writeStmt->bind(2, value);
		writeStmt->bind(3, completesInsertion);
		
		return writeStmt->step() == SQLITE_DONE;

	} else if(ca == ObjectPartiallyCached) {
		Sqlite::Statement::Resetter resetRead(*readStmt);

		// TODO: Should update the column using the blob api, not waste all this mem
		readStmt->bind(1, lookupKey);
		readStmt->step();
		int currentLen = readStmt->byteLength(1);
		int resultLen = currentLen + value.size();
		vector<uint8_t> newData(resultLen);
		readStmt->column(0, newData);
		std::copy(value.begin(), value.end(), newData.begin()+currentLen);
		
		eraseObject(obj_id);
		
		Sqlite::Statement::Resetter resetWrite(*writeStmt);
		writeStmt->bind(1, lookupKey);
		writeStmt->bind(2, newData);
		writeStmt->bind(3, completesInsertion);
		
		return writeStmt->step() == SQLITE_DONE;
		
		
	} else {
		throw std::logic_error("should not reach this point");
	}

}

bool
SqliteCache::
eraseObject(const ObjectId &obj_id)
{
	Sqlite::Statement::Resetter resetRemove(*removeStmt);
	removeStmt->bind(1, keyify(obj_id));
	return removeStmt->step() == SQLITE_DONE;
}

void
SqliteCache::
setMaxSize(uint64_t max_size)
{
	if(0 == max_size)
		db.run("delete from cache;");
	
	// TODO
}

uint64_t
SqliteCache::
getCurrentSize()
{
	return 0;
}


// Private
string 
SqliteCache::
keyify(const ObjectId &obj_id)
{
	vector<unsigned char> saltedObjectID(key.size()+obj_id.size());
	std::copy(key.begin(), key.end(), saltedObjectID.begin());
	std::copy(obj_id.begin(), obj_id.end(), saltedObjectID.begin()+key.size());

	unsigned char *saltedChars = &saltedObjectID[0];
	unsigned char digestedKeyChars[MD5_DIGEST_LENGTH+1];
	
	MD5(saltedChars, saltedObjectID.size(), digestedKeyChars);
	digestedKeyChars[MD5_DIGEST_LENGTH] = '\0';

	return string((char*)digestedKeyChars);
}




Cache *createCache(const string &path, const vector<uint8_t> &encryption_key)
{
	return new SqliteCache(path, encryption_key);
}