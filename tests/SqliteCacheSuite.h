/*
 *  SqliteCacheSuite.h
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-06-27.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */

#include <cxxtest/TestSuite.h>

#include "SqliteCache.h"

#include <iostream>
#include <algorithm>
using std::string;
using std::vector;

CXXTEST_ENUM_TRAITS( Cache::CacheAvailability,
										CXXTEST_ENUM_MEMBER( Cache::ObjectNotCached )
										CXXTEST_ENUM_MEMBER( Cache::ObjectCached )
										CXXTEST_ENUM_MEMBER( Cache::ObjectPartiallyCached )
);



class SqliteCacheSuite : public CxxTest::TestSuite {
public:
	vector<uint8_t> key;
	Cache *cache;

	SqliteCacheSuite() {
		string keyString = "1234";
		key = vector<uint8_t>(keyString.begin(), keyString.end());
		cache = createCache("/tmp/test.cache", key);
		cache->setMaxSize(0); // empty it
		cache->setMaxSize(UINT_MAX);
	}
	~SqliteCacheSuite() {
		delete cache;
	}
	// Avoid static initialization; sqlite3 won't be ready at SI time
	static SqliteCacheSuite *createSuite() { return new SqliteCacheSuite(); }
	static void destroySuite( SqliteCacheSuite *suite ) { delete suite; }

	
	// These depend on being run in-order

	void testAvailabilityOfNonExistingObject() {
		Cache::CacheAvailability ca = cache->objectIsAvailable(key);
		TS_ASSERT_EQUALS(ca, Cache::ObjectNotCached);
	}
	
	void testHasObjectOfNonExistingObject() {
		TS_ASSERT( ! cache->hasObject(string("DOES NOT EXIST")) );
	}
	
	void testReadingNonExistingObject() {
		vector<uint8_t> nothing;
		bool success = cache->readObject(key, nothing);
		TS_ASSERT(!success);
	}
	
	void testWritePartial() {
		bool success = cache->writeObject(key, key, false);
		TS_ASSERT(success);
	}
	
	void testPartialAvailability() {
		Cache::CacheAvailability ca = cache->objectIsAvailable(key);
		TS_ASSERT_EQUALS(ca, Cache::ObjectPartiallyCached);
	}
	
	void testHasObjectOfPartialObject() {
		TS_ASSERT( ! cache->hasObject(key) );
	}
	
	void testReadingPartial() {
		vector<uint8_t> partial;
		bool success = cache->readObject(key, partial);
		TS_ASSERT(success);
		TS_ASSERT_EQUALS(partial, key);
	}
	
	void testFinalizingPartial() {
		bool success = cache->writeObject(key, key, true);
		TS_ASSERT(success);
		
		vector<uint8_t> complete;
		success = cache->readObject(key, complete);
		TS_ASSERT(success);
		
		vector<uint8_t> expected = key;
		expected.insert(expected.end(), key.begin(), key.end());
		TS_ASSERT_EQUALS(expected, complete);
	}
	
	void testCompleteAvailability() {
		Cache::CacheAvailability ca = cache->objectIsAvailable(key);
		TS_ASSERT_EQUALS(ca, Cache::ObjectCached);
	}
	
	
	void testHasObjectOfCompleteObject() {
		TS_ASSERT( cache->hasObject(key) );
	}
	
	
	void testFinalizingAlreadyFinalized() {
		TS_ASSERT_THROWS(cache->writeObject(key, key, true),
										 Cache::AppendingToCompletedObjectException);
	}
	
	void testGettingSize() {
		// Cache currently contains 8 bytes ("12341234")
		TS_ASSERT_EQUALS(cache->getCurrentSize(), 8);
	}
	
	void testOversteppingCacheSize() {
		Cache::ObjectId key2 = Cache::ObjectId(string("key 2"));
		
		cache->setMaxSize(8+3);
		
		// Inserting a new object of size 4 should push the old one out and this
		// write should succeed
		bool success;
		TS_ASSERT_THROWS_NOTHING(success = cache->writeObject(key2, key));
		TS_ASSERT(success);
		
		// Should have been pushed out
		TS_ASSERT( ! cache->hasObject(key) );
		
		// Should have replaced it
		TS_ASSERT( cache->hasObject(key2) );
		
		TS_ASSERT_EQUALS( cache->getCurrentSize(), 4 );
	}
	
	
};