/*
 *  performancetest.cc
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-07-07.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */
#include "Cache.h"
#include <sys/time.h>
double getTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec/1000000.;
}

vector<uint8_t> vs(const string &s) {
	return vector<uint8_t>(s.begin(), s.end());
}

void perftest(int objCount, int objSize) {
	system("rm -f /tmp/performance.cache");
	Cache *cache = createCache("/tmp/performance.cache", vs("my key"));

	double start = getTime();
	
	for(int i = 0; i < objCount; i++) {
		char key[9];
		sprintf(key, "key%d", i);
		
		vector<uint8_t> value(objSize);
		std::copy(key, key+9, value.begin()); 
		
		cache->writeObject(key, value);
	}
	
	double afterCreate = getTime();
	
	vector<uint8_t> oneFetched;
	assert(cache->readObject("key3", oneFetched));
	
	double afterFetch = getTime();
	
	{
		double timeToCreate = afterCreate-start;
		double timeToFetch = afterFetch-start-timeToCreate;
		double creationTimePerObj = timeToCreate/objCount;
#if 1
		printf("%7d objs @ %7db: create(%3.4fs/%3.4fs) fetch(%3.4fs): %s\n", 
					 objCount, objSize, timeToCreate, creationTimePerObj, timeToFetch,
					 &(*oneFetched.begin()));
#else
		printf("%d\t%f\n", objCount, creationTimePerObj);
#endif
		
	}
	
	delete cache;
}

int main() {
	perftest(500, 100);
	perftest(500, 1000);
	perftest(500, 2000);
	perftest(500, 5000);
	perftest(500, 10000);

	perftest(10, 200);
	perftest(100, 200);
	perftest(1000, 200);
	
	perftest(10, 1000);
	perftest(100, 1000);
	perftest(1000, 1000);
	
	
	perftest(10000, 200);
	perftest(50000, 200);
	perftest(10000, 1000);
	perftest(50000, 1000);
	
	
	return 0;
}