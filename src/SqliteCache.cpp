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
static string cacheSchema = 
"create table if not exists cache ( "
"  object_id blob unique primary key, "
"  data blob not null, "
"  datahash blob not null, "
"  datasize int not null, "
"  completed bool not null default false, "
"  accessed_at int32_t not null" 
");";


SqliteCache::
SqliteCache(const string &path, const vector<uint8_t> &encryption_key)
: db(path), key(encryption_key), max_size(ULONG_LONG_MAX)
{
	db.run(cacheSchema);
	hasStmt = db.prepareFirst("select completed from cache where object_id = ?;");
	readStmt = db.prepareFirst("select data, datahash, completed from cache where object_id = ?;");
	writeStmt = db.prepareFirst("insert into cache (object_id, data, datahash, datasize, completed, accessed_at) values (?, ?, ?, ?, ?, ?);");
	removeStmt = db.prepareFirst("delete from cache where object_id = ?;");
	touchStmt = db.prepareFirst("update cache set accessed_at = ? where object_id = ?;");
	sizeStmt = db.prepareFirst("select sum(datasize) from cache;");
	sizeRemovableStmt = db.prepareFirst("select sum(datasize) from cache where completed = 1;");
	setCompletedStmt = db.prepareFirst("update cache set completed = ? where object_id = ?");
	findRowidStmt = db.prepareFirst("select rowid from cache where object_id = ?;");
	chksumStmt = db.prepareFirst("update cache set datahash = ? where object_id = ?;");
	oldestRemovablesStmt = db.prepareFirst("select rowid, datasize from cache where completed = 1 order by accessed_at asc;");
	removeByRowidStmt = db.prepareFirst("delete from cache where rowid = ?;");
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
	
	bool completed = readStmt->intColumn(2);
	if(completed) {
		vector<uint8_t> recordedHash;
		readStmt->column(1, recordedHash);
		
		vector<uint8_t> currentHash = SqliteCache::hash(result);
		
		if(currentHash != recordedHash)
			return false;		
	}
	
	Sqlite::Statement::Resetter resetTouch(*touchStmt);
	touchStmt->bind(1, time(NULL));
	touchStmt->step();
	
	return true;
}

bool
SqliteCache::
writeObject(const ObjectId &obj_id, 
						const vector<uint8_t>	 &value)
{
	::Cache::Partial::Ptr cacheObj = this->partial(obj_id, value.size());
	if(!cacheObj.get())
		return false;
	cacheObj->write(value);
	return true;
}

::Cache::Partial::Ptr
SqliteCache::
partial(const ObjectId &obj_id, uint64_t bytesToReserve)
{
	CacheAvailability currentCA = objectIsAvailable(obj_id);
	// If another Partial object exists for this obj_id, we won't interfere with it.
	if(currentCA == ObjectPartiallyCached)
		return Partial::Ptr(NULL);
	// If the object already exists, mark it as partial and resize.
	if(currentCA == ObjectCached) {
		Sqlite::Statement::Resetter r1(*setCompletedStmt);
		setCompletedStmt->bind(1, false);
		setCompletedStmt->bind(2, keyify(obj_id));
		if(setCompletedStmt->step() != SQLITE_DONE)
			return Partial::Ptr(NULL);
		
		Partial::Ptr partialPtr(new SCPartial(this, obj_id));
		return partialPtr;
	}
	// The final possible case is if the object doesn't exist.
	
	// Make sure it'll fit
	if(!ensureFreeSpace(bytesToReserve))
		return Partial::Ptr(NULL);
	
	// If it will, create the object, its partial, and reserve space.
	{
		Sqlite::Statement::Resetter resetWrite(*writeStmt);
		vector<uint8_t> nothing;
		writeStmt->bind(1, keyify(obj_id));
		writeStmt->bindZeroBlob(2, bytesToReserve);
		writeStmt->bindZeroBlob(3, MD5_DIGEST_LENGTH);
		writeStmt->bind(4, bytesToReserve);
		writeStmt->bind(5, false);
		writeStmt->bind(6, time(NULL));
		
		if(writeStmt->step() != SQLITE_DONE)
			return Partial::Ptr(NULL);
		
		Partial::Ptr partialPtr(new SCPartial(this, obj_id));
		return partialPtr;		
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
setMaxSize(uint64_t max_size_)
{
	if(0 == max_size)
		db.run("delete from cache;");
	
	// If we're shrinking, we might need to free up some space
	if(max_size_ < max_size)
		ensureMaxUsage(max_size_);
	
	max_size = max_size_;
	
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
	Sqlite::Statement::Resetter resetSize(*sizeStmt);
	sizeStmt->step();
	return sizeStmt->int64Column(0);
}
uint64_t
SqliteCache::
getRemovableSize()
{
	Sqlite::Statement::Resetter r1(*sizeRemovableStmt);
	sizeRemovableStmt->step();
	return sizeRemovableStmt->int64Column(0);	
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
SqliteCache::
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

	return SqliteCache::hash(saltedObjectID);
}

bool
SqliteCache::
ensureFreeSpace(uint64_t freeByteCount)
{
	// Required free bigger than max_usage? Impossible to fulfill
	if(freeByteCount > max_size)
		return false;
	return ensureMaxUsage(max_size-freeByteCount);
}

bool 
SqliteCache::
ensureMaxUsage(uint64_t maxUsageByteCount)
{	
	uint64_t currentSize = getCurrentSize();
	// Enough space is already free?
	if(currentSize <= maxUsageByteCount)
		return true;
	
	// Okay, we need to free stuff.
	
	// Perhaps even freeing wouldn't help, because
	// we have big partials (which we may not purge)?
	if(currentSize-getRemovableSize() > maxUsageByteCount)
		return false;
	
	// Let's start purging.
	Sqlite::Statement::Resetter r1(*oldestRemovablesStmt);
	do {
		if(oldestRemovablesStmt->step() == SQLITE_DONE)
			return false; // Huh, we are not done, but out of objects to remove!
		
		int64_t rowid = oldestRemovablesStmt->int64Column(0);
		int64_t freedBytes = oldestRemovablesStmt->int64Column(1);
		
		Sqlite::Statement::Resetter r2(*removeByRowidStmt);
		removeByRowidStmt->bind(1, rowid);
		removeByRowidStmt->step();
		
		currentSize -= freedBytes;

	} while(currentSize > maxUsageByteCount);
	
	return true;
}



Cache *createCache(const string &path, const vector<uint8_t> &encryption_key)
{
	return new SqliteCache(path, encryption_key);
}