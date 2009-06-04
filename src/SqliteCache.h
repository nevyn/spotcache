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
#include <sqlite3.h>

class SqliteCache : public Cache {
	friend Cache *createCache(const string &path, const vector<uint8_t> &encryption_key); 
public:
	// --- Types ---
	class SqliteConstructionError : public std::runtime_error {
	public:
		SqliteConstructionError(string err) : std::runtime_error(err) {}
		
		int errorCode;
	};
	
	// --- Methods ---
	~SqliteCache();
	
	// --- Inherited methods ---
	bool hasObject(const ObjectId &obj_id); 
	CacheAvailability objectIsAvailable(const ObjectId &obj_id);
	bool readObject(const ObjectId &obj_id, vector<uint8_t> &result);
	bool writeObject(const ObjectId &obj_id, 
	                         const vector<uint8_t>	 &value,
													 bool completesInsertion = true); 
	bool eraseObject(const ObjectId &obj_id);
	virtual void setMaxSize(uint64_t max_size); 
	virtual uint64_t getCurrentSize();
	
	// --- Implementation detail methods --
protected:
	string keyify(const ObjectId &obj_id);
	
	// --- Ivars ---
protected:
	SqliteCache(const string &path, const vector<uint8_t> &encryption_key);
	sqlite3 *db;
	const vector<uint8_t> key;

	
};

#endif