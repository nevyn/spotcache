#include "Cache.h"

bool Cache::hasObject(const ObjectId &obj_id)
{
	return objectIsAvailable(obj_id) == ObjectCached;
}