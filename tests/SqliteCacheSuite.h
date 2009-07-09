/*
 *  SqliteCacheSuite.h
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-06-27.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */

#include <cxxtest/TestSuite.h>
#define IS_TESTING
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
	Cache::ObjectId key;
	Cache *cache;

	SqliteCacheSuite() : key("1234") {
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
		TS_ASSERT( ! cache->hasObject("DOES NOT EXIST") );
	}
	
	void testReadingNonExistingObject() {
		vector<uint8_t> nothing;
		bool success = cache->readObject(key, nothing);
		TS_ASSERT(!success);
	}
	
	Cache::Partial::Ptr partial;
	
	void testWritePartial() {
		partial = cache->partial(key, 8);
		partial->append(key);
	}
	
	void testPartialAvailability() {
		Cache::CacheAvailability ca = cache->objectIsAvailable(key);
		TS_ASSERT_EQUALS(ca, Cache::ObjectPartiallyCached);
	}
	
	void testHasObjectOfPartialObject() {
		TS_ASSERT( ! cache->hasObject(key) );
	}
	
	void testReadingPartial() {
		vector<uint8_t> sofar;
		bool success = cache->readObject(key, sofar);
		TS_ASSERT(success);
		
		TS_ASSERT_EQUALS(sofar.size(), 8);
		
		// Contents of the range we haven't written to yet is undefined; cut out
		// the beginning.
		vector<uint8_t> first4(sofar.begin(), sofar.begin()+4);
		

		TS_ASSERT_EQUALS(first4, key);
	}
	
	void testTwoPartialsOneObject() {
		Cache::Partial::Ptr partial2 = cache->partial(key, 8);
		TS_ASSERT(partial2.get() == NULL);
	}
	
	void testFinalizingPartial() {
		partial->append(key);
		// Finalize the partial object, effectively removing the 'partial'
		// state from the cache object.
		partial.reset();
		
		vector<uint8_t> complete;
		bool success = cache->readObject(key, complete);
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
	
	void testTurningExistingObjectIntoPartial() {
		Cache::CacheAvailability ca = cache->objectIsAvailable(key);
		TS_ASSERT_EQUALS(ca, Cache::ObjectCached);
		{
			Cache::Partial::Ptr partial2 = cache->partial(key, 8);
			Cache::CacheAvailability ca2 = cache->objectIsAvailable(key);
			TS_ASSERT_EQUALS(ca2, Cache::ObjectPartiallyCached);			
		}
		Cache::CacheAvailability ca3 = cache->objectIsAvailable(key);
		TS_ASSERT_EQUALS(ca3, Cache::ObjectCached);
	}
	
		
	void testGettingSize() {
		// Cache currently contains 8 bytes ("12341234")
		TS_ASSERT_EQUALS(cache->getCurrentSize(), 8);
	}
	
	void testOversteppingCacheSize() {
		Cache::ObjectId key2 = "key 2";
		
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
		
		// Should push that object out as well
		cache->setMaxSize(3); 
		TS_ASSERT( ! cache->hasObject(key2) );
		
		TS_ASSERT_EQUALS( cache->getCurrentSize(), 0 );
		
		// Restore normality
		cache->setMaxSize(ULONG_LONG_MAX); 
	}
	
	// End in-order dependency
	
	void testCorruptingStore() {
		Cache::ObjectId someKey = "this'll become corrupted";
		TS_ASSERT( cache->writeObject(someKey, someKey) == true);
		
		vector<uint8_t> value;
		
		TS_ASSERT( cache->readObject(someKey, value) == true);
		
		Cache::ObjectId realKey = ((SqliteCache*)cache)->keyify(someKey);
		
		{
			Sqlite store("/tmp/test.cache");
			Sqlite::Statement::Ptr corruptStmt = store.prepareFirst("update cache set data = ? where object_id = ?;");
			corruptStmt->bind(1, "lol");
			corruptStmt->bind(2, realKey);
			TS_ASSERT_EQUALS(corruptStmt->step(), SQLITE_DONE);
		}
		
		TS_ASSERT( cache->hasObject(someKey) == true );
		
		TS_ASSERT( cache->readObject(someKey, value) == false);
	}
	
	
};