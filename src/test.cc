/*
 *  test.cc
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-06-03.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */
#include "Cache.h"
#include <iostream>
#include <algorithm>
#include "Sqlite.h"
#include <stdlib.h>

using std::cout;
using std::endl;

string asString(vector<uint8_t> v) {
	string s(v.begin(), v.end());
	return s;
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	
	// Rudimentary test of Sqlite
	
	Sqlite db("/tmp/test.sqlite");
	db.run("drop table if exists foo;");
	db.run("create table foo(i int);");
	
	Sqlite::Statement::Ptr insert = db.prepareFirst("insert into foo values (?);");
	insert->bind(1, rand());
	insert->step();
	insert->reset();
	insert->bind(1, rand());
	insert->step();
	
	
	
	// Rudimentary test of SqliteCache
	
	system("rm -f /tmp/test.cache");
	
	string keyString = "asdf";
	vector<uint8_t> key(keyString.begin(), keyString.end());
	
	Cache *cache = createCache("/tmp/test.cache", key);
	
	cout << "Availability for non-existing obj should be ObjectNotCached" << endl;
	Cache::CacheAvailability ca = cache->objectIsAvailable(key);
	assert(ca == Cache::ObjectNotCached);
	
	cout << "Reading non-existing object should fail" << endl;
	vector<uint8_t> nothing;
	bool success = cache->readObject(key, nothing);
	assert(success == false);
	
	cout << "Writing partial new object should succeed" << endl;
	success = cache->writeObject(key, key, false);
	assert(success == true);
	
	cout << "Availability should now be ObjectPartiallyCached" << endl;
	ca = cache->objectIsAvailable(key);
	assert(ca == Cache::ObjectPartiallyCached);
	
	cout << "Reading partial object should succeed" << endl;
	vector<uint8_t> partial;
	success = cache->readObject(key, partial);
	assert(success);
	cout << "The returned partial should equal what was previously inserted" <<endl;
	assert(partial == key);
	
	cout << "Writing second half of object and finalizing it should succeed" << endl;
	success = cache->writeObject(key, key, true);
	assert(success == true);
	
	cout << "Writing after finalizing should fail" << endl;
	bool failed = false;
	try {
		failed = ! cache->writeObject(key, key, true);
	}
	catch (Cache::AppendingToCompletedObjectException &e) {
		failed = true;
	}
	assert(failed == true);
	
	cout << "Reading finalized object should succeed" << endl;
	vector<uint8_t> complete;
	success = cache->readObject(key, complete);
	assert(success);
	
	cout << "Finalized object should equal sum of inserts" << endl;
	vector<uint8_t> expected = key;
	expected.insert(expected.end(), key.begin(), key.end());
	assert(expected == complete);
	
	
	
	delete cache;
	
	return 0;
}