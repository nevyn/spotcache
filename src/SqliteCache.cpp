/*
 *  SqliteCache.cpp
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-06-03.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */

#include "SqliteCache.h"
extern "C" {
#include "md5.h"
}
#include <algorithm>
#include <time.h>

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

static string metaSchema = 
"create table if not exists meta ("
"  max_size int not null "
");";

SqliteCache::
SqliteCache(const string &path, const vector<uint8_t> &encryption_key)
: db(path), key(encryption_key), max_size(LONG_LONG_MAX)
{
	db.run(cacheSchema);
	db.run(metaSchema);
	hasStmt = db.prepareFirst("select completed from cache where object_id = ?;");
	readStmt = db.prepareFirst("select data, datahash, completed, datasize from cache where object_id = ?;");
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
	specificSizeStmt = db.prepareFirst("select datasize from cache where object_id = ?");
	
	readMeta();
	
	current_size = -1;
	getCurrentSize(); // sets current_size
}

SqliteCache::
~SqliteCache()
{
	writeMeta();
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
	
	bool completed = (bool)readStmt->intColumn(2);
	if(completed) {
		vector<uint8_t> recordedHash;
		readStmt->column(1, recordedHash);
		
		vector<uint8_t> currentHash = SqliteCache::hash(result);
		
		if(currentHash != recordedHash)
			return false;
	}
#if USE_ONDISK_ENCRYPTION
	int64_t size = readStmt->int64Column(3);
	decrypt(result.begin(), result.begin() + size);
#endif
	
	
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

static const int md5_digest_length = 16;

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
		writeStmt->bindZeroBlob(3, md5_digest_length);
		writeStmt->bind(4, bytesToReserve);
		writeStmt->bind(5, false);
		writeStmt->bind(6, time(NULL));
		
		if(writeStmt->step() != SQLITE_DONE)
			return Partial::Ptr(NULL);
		
		current_size += bytesToReserve;
		
		Partial::Ptr partialPtr(new SCPartial(this, obj_id));
		return partialPtr;		
	}
}

bool
SqliteCache::
eraseObject(const ObjectId &obj_id)
{
	int64_t size = 0;
	{
		Sqlite::Statement::Resetter r1(*specificSizeStmt);
		specificSizeStmt->bind(1, keyify(obj_id));
		int status = specificSizeStmt->step();
		if(status != SQLITE_ROW)
			return false;
		size = specificSizeStmt->int64Column(0);
	}
	
	Sqlite::Statement::Resetter r2(*removeStmt);
	removeStmt->bind(1, keyify(obj_id));
	int status = removeStmt->step();
	if(status == SQLITE_DONE) {
		current_size -= size;
		return true;
	} else {
		return false;
	}
}

void
SqliteCache::
setMaxSize(int64_t max_size_)
{
	if(0 == max_size) {
		db.run("delete from cache;");
		current_size = 0;
	}
	
	// If we're shrinking, we might need to free up some space
	if(max_size_ < max_size) {
		ensureMaxUsage(max_size_);
		
		// Since we never grow beyond max_size, the sqlite cachesize+freelist will never be
		// bigger than max_size. If we decrease max_size however, we must vacuum
		// to make the cachesize+freelist smaller than max_size.
		// By not autovacuuming, we allow sqlite to optimize the free space
		db.run("VACUUM;");		
	}
		
	max_size = max_size_;
	
	writeMeta();
}

uint64_t
SqliteCache::
getCurrentSize()
{
	if(current_size == -1) {
		Sqlite::Statement::Resetter resetSize(*sizeStmt);
		sizeStmt->step();
		current_size = sizeStmt->int64Column(0);
	}
	return current_size;
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
	
	//assert(timeStmt->hasData());
	return timeStmt->intColumn(0);
}


// Private

vector<uint8_t>
SqliteCache::
hash(const vector<uint8_t> &data)
{
	vector<uint8_t> hashed(md5_digest_length);
	MD5_CTX ctx;
	MD5Init(&ctx);
	MD5Update(&ctx, const_cast<uint8_t*>(&(*data.begin())), data.size());
	MD5Final(&(*hashed.begin()), &ctx);
	return hashed;
}

// How's THIS for cryptographic weakness!
void
SqliteCache::
 encrypt(vector<uint8_t>::iterator from,  vector<uint8_t>::iterator to, uint64_t offset)
{
#if USE_ONDISK_ENCRYPTION
	for(vector<uint8_t>::iterator it = from;
			it != to;
			it++, offset++)
	{
		*it += key[offset % key.size()];
	}
#endif
}
void
SqliteCache::
decrypt(vector<uint8_t>::iterator from,  vector<uint8_t>::iterator to, uint64_t offset)
{
#if USE_ONDISK_ENCRYPTION
	for(vector<uint8_t>::iterator it = from;
			it != to;
			it++, offset++)
	{
		*it -= key[offset % key.size()];
	}
#endif
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
	// Enough space is already free?
	if(current_size <= maxUsageByteCount)
		return true;
	
	// Okay, we need to free stuff.
	
	// Perhaps even freeing wouldn't help, because
	// we have big partials (which we may not purge)?
	if(current_size-getRemovableSize() > maxUsageByteCount)
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
		
		current_size -= freedBytes;

	} while(current_size > maxUsageByteCount);
	
	return true;
}

void
SqliteCache::
readMeta()
{
	Sqlite::Statement::Ptr metaRead = db.prepareFirst("select max_size from meta;");
	int status = metaRead->step();
	
	// Nothing saved? Just return and have it written from ~SqliteCache
	if(status != SQLITE_ROW)
		return;
	
	setMaxSize(metaRead->int64Column(0));
}

void
SqliteCache::
writeMeta()
{
	db.run("delete from meta;");
	Sqlite::Statement::Ptr metaWrite = db.prepareFirst("insert into meta (max_size) values (?);");
	metaWrite->bind64(1, max_size);
	metaWrite->step();
}



Cache *createCache(const string &path, const vector<uint8_t> &encryption_key)
{
	return new SqliteCache(path, encryption_key);
}