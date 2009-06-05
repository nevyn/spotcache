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

int main(int argc, char *argv[])
{
	srand(time(NULL));
	
	// Rudimentary test of Sqlite
	
	Sqlite db("/tmp/test.sqlite");
	db.run("drop table if exists foo;");
	db.run("create table foo(i int);");
	
	Sqlite::Statement::Ptr insert = db.prepareFirst("insert into foo values (?);");
	insert->bind(1, rand());
	insert->stepUntilNotBusy();
	insert->reset();
	insert->bind(1, rand());
	insert->stepUntilNotBusy();
	
	// Rudimentary test of SqliteCache
	
	string keyString = "asdf";
	vector<uint8_t> key(keyString.size());
	std::copy(keyString.begin(), keyString.end()-1, key.begin());	
	
	Cache *cache = createCache("/tmp/test.cache", key);
	cache->writeObject(key, key, true);

	
	delete cache;
	
	return 0;
}