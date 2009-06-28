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

vector<uint8_t> hash(const vector<uint8_t> &data);

/// Schema
static string schema = 
"create table if not exists cache ( "
"  object_id blob unique primary key, "
"  data blob not null, "
"  datahash blob not null, "
"  completed bool not null default false, "
"  accessed_at int32_t not null" 
");";

SqliteCache::
SqliteCache(const string &path, const vector<uint8_t> &encryption_key)
: db(path), key(encryption_key)
{
	db.run(schema);
	hasStmt = db.prepareFirst("select completed from cache where object_id = ?;");
	readStmt = db.prepareFirst("select data, datahash from cache where object_id = ?;");
	writeStmt = db.prepareFirst("insert into cache (object_id, data, datahash, completed, accessed_at) values (?, ?, ?, ?, ?);");
	removeStmt = db.prepareFirst("delete from cache where object_id = ?;");
	touchStmt = db.prepareFirst("update cache set accessed_at = ? where object_id = ?;");
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
	
	if( ! readStmt->hasData())
		return false;
	
	
	readStmt->column(0, result);
	vector<uint8_t> recordedHash;
	readStmt->column(1, recordedHash);
	
	vector<uint8_t> currentHash = hash(result);
	
	if(currentHash != recordedHash)
		return false;
	
	Sqlite::Statement::Resetter resetTouch(*touchStmt);
	touchStmt->bind(1, time(NULL));
	touchStmt->step();	
	
	return true;
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
	
	ObjectId lookupKey = keyify(obj_id);

	
	if(ca == ObjectNotCached) {
		vector<uint8_t> valueHash = hash(value);

		Sqlite::Statement::Resetter resetWrite(*writeStmt);
		writeStmt->bind(1, lookupKey);
		writeStmt->bind(2, value);
		writeStmt->bind(3, valueHash);
		writeStmt->bind(4, completesInsertion);
		writeStmt->bind(5, time(NULL));
		
		return writeStmt->step() == SQLITE_DONE;

	} else if(ca == ObjectPartiallyCached) {
		Sqlite::Statement::Resetter resetRead(*readStmt);
		
		// TODO: Should update the column using the blob api, not waste all this mem
		readStmt->bind(1, lookupKey);
		readStmt->step();
		int32_t currentLen = readStmt->byteLength(0);
		int32_t resultLen = currentLen + value.size();
		vector<uint8_t> newData(resultLen);
		readStmt->column(0, newData);
		std::copy(value.begin(), value.end(), newData.begin()+currentLen);
		
		eraseObject(obj_id);
		
		vector<uint8_t> newDataHash = hash(newData);
		
		Sqlite::Statement::Resetter resetWrite(*writeStmt);
		writeStmt->bind(1, lookupKey);
		writeStmt->bind(2, newData);
		writeStmt->bind(3, newDataHash);
		writeStmt->bind(4, completesInsertion);
		writeStmt->bind(5, time(NULL));
		
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
	
	// Since we never grow beyond max_size, the sqlite cachesize+freelist will never be
	// bigger than max_size. If we decrease max_size however, we must vacuum
	// to make the cachesize+freelist smaller than max_size.
	// By not autovacuuming, we allow sqlite to optimize the free space
	db.run("VACUUM;");
}

uint64_t
SqliteCache::
getCurrentSize()
{
	return 0;
}

time_t
SqliteCache::
accessTimeOfObject(const ObjectId &obj_id)
{
	Sqlite::Statement::Ptr timeStmt = db.prepareFirst("select accessed_at from cache where object_id = ?;");
	timeStmt->bind(1, keyify(obj_id));
	timeStmt->step();
	
	assert(timeStmt->hasData());
	return timeStmt->intColumn(0);
}


// Private

vector<uint8_t>
hash(const vector<uint8_t> &data)
{
	vector<uint8_t> hashed(MD5_DIGEST_LENGTH);
	MD5(&(*data.begin()),
			data.size(),
			&(*hashed.begin()));
	return hashed;
}

Cache::ObjectId 
SqliteCache::
keyify(const ObjectId &obj_id)
{
	ObjectId saltedObjectID(key.size()+obj_id.size());
	std::copy(key.begin(), key.end(), saltedObjectID.begin());
	std::copy(obj_id.begin(), obj_id.end(), saltedObjectID.begin()+key.size());

	return hash(saltedObjectID);
}




Cache *createCache(const string &path, const vector<uint8_t> &encryption_key)
{
	return new SqliteCache(path, encryption_key);
}