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
	
	vector<uint8_t> value(objSize);
	
	for(int i = 0; i < objCount; i++) {
		char key[9];
		sprintf(key, "key%d", i);
		
		assert(cache->writeObject(key, value));
	}
	
	double afterCreate = getTime();
	
	vector<uint8_t> oneFetched;
	for(int i = 0; i < objCount; i++) {
		char key[9];
		sprintf(key, "key%d", i);
		
		assert(cache->readObject(key, oneFetched));
	}
	
	double afterFetch = getTime();
	
	{
		double timeToCreate = afterCreate-start;
		double creationTimePerObj = timeToCreate/objCount;
		double creationTimePerByte = creationTimePerObj/objSize;
		
		double timeToFetch = afterFetch-start-timeToCreate;
		double fetchTimePerObj = timeToFetch/objCount;
		double fetchTimePerByte = fetchTimePerObj/objSize;
#if 1
		printf("%7d objs @ %7db: create(%9.5fs/%9.5fs/%14.10fs) fetch(%9.5fs/%9.5fs/%14.10fs)\n", 
					 objCount, objSize, 
					 timeToCreate, creationTimePerObj, creationTimePerByte,
					 timeToFetch, fetchTimePerObj, fetchTimePerByte);
#else
		printf("%d\t%f\n", objCount, creationTimePerObj);
#endif
		
	}
	
	delete cache;
}

int main() {
	printf("By size\n");
	perftest(500, 100);
	perftest(500, 1000);
	perftest(500, 2000);
	perftest(500, 5000);
	perftest(500, 10000);
	perftest(500, 20000);
	perftest(500, 50000);

	printf("By object count\n");
	perftest(10,    1000);
	perftest(100,   1000);
	perftest(1000,  1000);
	perftest(2000,  1000);
	perftest(5000,  1000);
	perftest(10000, 1000);
	perftest(20000, 1000);
	perftest(50000, 1000);
	
	
	return 0;
}