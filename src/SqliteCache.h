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
	
	// --- Methods ---
	~SqliteCache();
	
	// --- Inherited methods ---
	bool hasObject(const ObjectId &obj_id); 
	CacheAvailability objectIsAvailable(const ObjectId &obj_id);
	bool readObject(const ObjectId &obj_id, vector<uint8_t> &result);
	bool writeObject(const ObjectId &obj_id, 
	                         const vector<uint8_t>	 &value,
													 bool completesInsertion = true) throw(AppendingToCompletedObjectException); 
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
	
	// --- Ivars ---
protected:
	const vector<uint8_t> key;
	
	Sqlite db;
	Sqlite::Statement::Ptr hasStmt, readStmt, writeStmt, removeStmt, touchStmt;
	
	
};

#endif