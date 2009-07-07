/*
 *  SqliteCache+Partial.cpp
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-07-06.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */

#include "SqliteCache.h"
#include <stdexcept>

SqliteCache::SCPartial::
SCPartial() : cache(NULL), obj_id(""){
	throw std::logic_error("illegal");
}

SqliteCache::SCPartial::
SCPartial(SqliteCache *cache_, const ObjectId &obj_id_)
: cache(cache_), _positionIndicator(0), obj_id(obj_id_)
{
	Sqlite::Statement::Resetter r1(*cache->findRowidStmt);
	cache->findRowidStmt->bind(1, cache->keyify(obj_id));
	int status = cache->findRowidStmt->step();
	if(status != SQLITE_ROW)
		throw std::logic_error("Making partial of object that doesn't exist; this should be impossible");
	int64_t rowid = cache->findRowidStmt->int64Column(0);
	
	blob = cache->db.blob("cache", "data", rowid);
}

SqliteCache::SCPartial::
~SCPartial()
{
	// Update the checksum to match the new data
	vector<uint8_t> everything = blob->read();
	vector<uint8_t> datahash = SqliteCache::hash(everything);
	
	Sqlite::Statement::Resetter r1(*(cache->chksumStmt));
	cache->chksumStmt->bind(1, datahash);
	cache->chksumStmt->bind(2, cache->keyify(obj_id));
	cache->chksumStmt->step();
	
	// Touch the object
	Sqlite::Statement::Resetter resetTouch(*cache->touchStmt);
	cache->touchStmt->bind(1, time(NULL));
	cache->touchStmt->step();	
	
	// We're done with the partial, mark it as such
	Sqlite::Statement::Resetter r2(*(cache->setCompletedStmt));
	cache->setCompletedStmt->bind(1, true);
	cache->setCompletedStmt->bind(2, cache->keyify(obj_id));
	cache->setCompletedStmt->step();
}

uint64_t 
SqliteCache::SCPartial::
positionIndicator()
{
	return _positionIndicator;
}

uint64_t
SqliteCache::SCPartial::
seek(int64_t to, int whence)
{
	switch (whence) {
		case SEEK_SET:
			_positionIndicator = to;
			return _positionIndicator;
		case SEEK_CUR:
			_positionIndicator += to;
			return _positionIndicator;
		case SEEK_END:
			_positionIndicator = size()+to;
			return _positionIndicator;
		default:
			return _positionIndicator;
	}
}


void
SqliteCache::SCPartial::
append(const vector<uint8_t> &value)
{
	write(value.begin(), value.end(), _positionIndicator);
	_positionIndicator += value.size();
}

void
SqliteCache::SCPartial::
read(vector<uint8_t>::iterator destination, 
		 int32_t bytesToRead,
		 int32_t offset)
{
	blob->read(destination, bytesToRead, offset);
}

void 
SqliteCache::SCPartial::
write(const vector<uint8_t>::const_iterator from,
			const vector<uint8_t>::const_iterator to,
			int64_t destOffset)
{
	blob->write(from, to, destOffset);
}

uint64_t
SqliteCache::SCPartial::
size()
{
	return blob->size();
}

