#include "Cache.h"

bool Cache::hasObject(const ObjectId &obj_id)
{
	return objectIsAvailable(obj_id) == ObjectCached;
}

vector<uint8_t>
Cache::Partial::
read(int64_t bytesToRead, int64_t offset)
{
	if(bytesToRead == -1) bytesToRead = size();
	vector<uint8_t> dest(bytesToRead);
	read(dest.begin(), bytesToRead, offset);
	return dest;	
}

void 
Cache::Partial::
write(const vector<uint8_t> &source, int64_t offset)
{
	write(source.begin(), source.end(), offset);
}