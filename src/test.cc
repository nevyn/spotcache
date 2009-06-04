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

int main(int argc, char *argv[])
{
	string keyString = "asdf";
	vector<uint8_t> key(keyString.size());
	std::copy(keyString.begin(), keyString.end()-1, key.begin());

	Cache *cache = createCache("/tmp/test.cache", key);
	cache->writeObject(key, key, true);
	
	
	delete cache;
	return 0;
}