SpotCache
========================

SpotCache is a Sqlite3 backed caching class in C++ by [Joachim Bengtsson](mailto:joachimb@gmail.com) as recruitment programming assignment for Spotify. I realize you might want to have tested my problem solving abilities, or my algorithmic knowledge. I chose the pragmatic approach and went for a problem solving approach; my implementation is thus probably a poor measure of the latter. I'll reimplement it if that's what you were really interested in.

Usage overview
-------------------
An Xcode 3.1 project, a Makefile and a Visual Studio 2008 solution is included. Sqlite3, a portable stdint (pstdint), the original md5 implementation, and cxxtest are bundled as dependencies, and nothing needs to be installed on a development or production machine to use this code.

However, Cxxtest requires python or perl to generate tests.cpp; If you don't change anything you will be fine with the generate in the repository, but to regenerate it you will need to install python or perl if that's not already available on your system. Then, before each build (preferably as an automatic build step, as in the Xcode project), run:
	python vendor/cxxtest/cxxtestgen.py --error-printer -o tests/tests.cpp \
	tests/SqliteDbSuite.h tests/SqliteTableSuite.h tests/SqliteCacheSuite.h



Sample usage:

	Cache *cache = createCache("/tmp/test.cache", vs("my key"));
	
	string key = "my object.png";
	vector<uint8_t> value = ...;
	assert(cache->writeObject(key, value));
	
	...
	
	vector<uint8_t> obj;
	assert(cache->readObject(key, obj));
	
	...
	
	{
	  HttpConnection conn = ...;
	  Cache::Partial::Ptr bigObject = cache->partial(conn.fileName(), header.size() + conn.payloadSize());
	  bigObject.seek(header.size(), SEEK_SET);
	  while(conn.isOpen()) {
	    bigObject.append(conn.read(512));
	  }
	  bigObject.write(footer, 0);
	}
	
	delete cache;


API documentation
-------------------
All header files are thoroughly documented although not doxygen compatible; refer to these for API details. If a method seems undocumented, check the superclass.

=== Changes in Cache.h ===

I have made changes in Cache.h from your original version. However, any client using the previous interface should be perfectly compatible with the new interface, as I've only added to it and not made any semantic changes. These changes all pertain in some way to having partial objects and incremental writing in the cache; while the instructions said this feature only needed designing, not implementation, I couldn't help but to take that as a challenge. Also, implementing it helped me understand why my original design was flawed. Here are the changes in Cache.h:

ObjectId is a class instead of an enum to make it easier to create from e g a string.

CacheAvailability is a new enum complementing the new method objectIsAvailable. The two first values of CacheAvailability are analogous to the return value from hasObject; the third value, ObjectPartiallyCached indicates that an object exists in the cache but is incomplete and should be treated as such. readObject will  return such an object even though it has not ben fully written yet.

The Partial class represents such an object, which exists in the cache but has not been completely written yet (for example, a download that is in progress). A Partial is created with the partial method of Cache. If called with an ObjectId of an object that didn't exist before, zeroes are written to the cache for this object, to reserve the space. If called with an ObjectId of an object that *did* exist, that object's status is changed to partial and will no longer be seen by clients only interested in completed objects. If called with an ObjectId of an object that did exist *and* was already marked as partial, the call will abort and return NULL.

When the Partial object is destroyed, the cache object it represents is hashed and marked as complete.

=== Object sizes ===

Note that while uint64_t is used here and there to represent sizes and offsets, sqlite uses the native signed int type to represent these concepts, and objects >2g in size might fail on platforms where sizeof(int)==4 (which is pretty much all of them).

Design rationale
-------------------
=== Iterations of the Partials interface ===

My first approach was very simple. I added the CacheAvailability enum, the objectIsAvailable method, and a third boolean argument to writeObject, saying whether this write completes that object and that it may be marked as such.

While this API was very simple (in true KISS tradition), it would conceptually fail if Cache is given a max size, and partial writes are interleaved with other objects; such another write might then trigger a prune, which would remove the incomplete object, and later partial writes wouldn't notice a thing, leaving an object with only the second half on disk. Also, this API meant space wasn't reserved and might run out while writing the partial (which means a partial write for an object might prune the already written parts of it!). This couldn't work.

Obviously I had some implicit concept here, of a transaction, and decided to make it explicit as an object; the result is the Partial class. I kept a positionIndicator() and an append() method, so that it could be used as easily as the old writeObject method. While at it, I added some random access read/write methods as well.

=== Using sqlite3 ===

I'm guessing the implementation you had in mind would be a tree of files on disk, together with a serializable hash for fast lookups. I felt walking down that road would mean duplicate state and lots of low-level code that would be prone to bugginess. If I had an alternative that felt good, the low level approach is not how I would do it if I was employed (as opposed to doing it as a mental exercise); it would be reinventing the wheel. I was thinking, if I used another, mature, store for the cache, I might get very fast lookups and plenty of optimizations, for free.

Sqlite is very well ported; used worldwide in many, many projects; in the public domain and thus very free; and incredibly well tested (45MLOC of testing code versus 64kLOC of application code). I decided to give it a go.

I'm *very* curious as to the performance of my code compares to a good files-and-hashes implementation.

=== The sqlite3 wrapper ===

... is very C-ish. Instead of returning a vector of results, it returns SQLITE_ROW and you have to step through the results and manually access the columns. While verbose, this interface is efficient, and frankly I didn't feel I needed a higher or simpler abstraction. Why wrap at all, then? For the memory management, type inference and exceptions.

Performance
-------------------
See "Performance measurements.txt"
