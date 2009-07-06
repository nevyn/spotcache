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
}

SqliteCache::SCPartial::
~SCPartial()
{
	// We're done with the partial, mark it as such
	
	// TODO: Write the checksum!
	
	Sqlite::Statement::Resetter r1(*(cache->setPartialityStmt));
	cache->setPartialityStmt->bind(1, true);
	cache->setPartialityStmt->bind(2, cache->keyify(obj_id));
	cache->setPartialityStmt->step();
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
	writeAt(_positionIndicator, value);
	_positionIndicator += value.size();
}
void
SqliteCache::SCPartial::
writeAt(uint64_t offset, const vector<uint8_t> &value)
{
	

}

void
SqliteCache::SCPartial::
resize(uint64_t new_size)
{
	
}

uint64_t
SqliteCache::SCPartial::
size()
{
	return 0;
}

