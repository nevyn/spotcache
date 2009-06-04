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
: key(encryption_key)
{
	int err = sqlite3_open(path.c_str(), &db);
	if(err != SQLITE_OK) {
		SqliteConstructionError exc = SqliteConstructionError(sqlite3_errmsg(db));
		exc.errorCode = err;
		sqlite3_close(db);
		throw exc;
	}
}

SqliteCache::
~SqliteCache()
{
	sqlite3_close(db);
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
	return ObjectNotCached;
}

bool
SqliteCache::
readObject(const ObjectId &obj_id, vector<uint8_t> &result)
{
	return false;
}

bool
SqliteCache::
writeObject(const ObjectId &obj_id, 
						const vector<uint8_t>	 &value,
						bool completesInsertion)
{
	keyify(obj_id);
	return false;
}

bool
SqliteCache::
eraseObject(const ObjectId &obj_id)
{
	return false;
}

void
SqliteCache::
setMaxSize(uint64_t max_size)
{
	;
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