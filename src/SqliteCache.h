#ifndef SQLITE_CACHE__H
#define SQLITE_CACHE__H

/*
 *  SqliteCache.h
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-06-03.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */

#include "Cache.h"
#include "Sqlite.h"

class SqliteCache : public Cache {
	friend Cache *createCache(const string &path, const vector<uint8_t> &encryption_key); 
#ifdef IS_TESTING
	friend class SqliteCacheSuite;
#endif
public:
	// --- Types ---
	
	class SCPartial : public Cache::Partial {
		friend class SqliteCache;
	public:		
		uint64_t positionIndicator();
		uint64_t seek(uint64_t, int whence);
		
		void append(const vector<uint8_t> &value);
		void writeAt(uint64_t offset, const vector<uint8_t> &value);
		
		void resize(uint64_t new_size);
		
		virtual ~SCPartial();
	protected:
		SCPartial();
		SCPartial(SqliteCache &cache);
		
	};
	// --- Methods ---
	~SqliteCache();
	
	// --- Inherited methods ---
	bool hasObject(const ObjectId &obj_id); 
	CacheAvailability objectIsAvailable(const ObjectId &obj_id);
	bool readObject(const ObjectId &obj_id, vector<uint8_t> &result);
	bool writeObject(const ObjectId &obj_id, const vector<uint8_t>	 &value);
	Partial::Ptr partial(const ObjectId &obj_id, uint64_t bytesToReserve);
	bool eraseObject(const ObjectId &obj_id);
	virtual void setMaxSize(uint64_t max_size); 
	virtual uint64_t getCurrentSize();
	
	time_t accessTimeOfObject(const ObjectId &obj_id);
	
	// --- Implementation detail methods --
protected:
	/// Salts the obj_id with this->key and hashes it. This is what will be used
	/// to do lookups in the db. The end result is that running this Cache
	/// instance with the wrong key yields a seemingly empty database.
	ObjectId keyify(const ObjectId &obj_id);
	SqliteCache(const string &path, const vector<uint8_t> &encryption_key);
	
	/// Deletes cache objects if needed to make sure the cache has freeByteCount
	/// bytes available before reaching maxSize. Uses ensureMaxUsage.
	/// @returns false if that's not possible without exceeding maxSize.
	bool ensureFreeSpace(uint64_t freeByteCount);
	/// Deletes cache objects if needed to make sure cache doesn't use more than
	/// maxUsageByteCount bytes.
	/// @returns false if that's not possible without exceeding maxSize.
	bool ensureMaxUsage(uint64_t maxUsageByteCount);
	
	
	// --- Ivars ---
protected:
	const vector<uint8_t> key;
	
	Sqlite db;
	Sqlite::Statement::Ptr hasStmt, readStmt, writeStmt, removeStmt, touchStmt,
												 sizeStmt, setPartialityStmt;
	
	uint64_t max_size;
};

#endif