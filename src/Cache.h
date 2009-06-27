#ifndef SPOT_CACHE__H
#define SPOT_CACHE__H

/// Joachim Bengtsson
/// joachimb@gmail.com
/// 2009-06-03

#include <stdexcept>
#include <vector>
#include <string>
using std::vector;
using std::string;


class Cache { 
public: 
	class ObjectId : public vector<uint8_t> {
	public:
		ObjectId(const string &other) : vector<uint8_t>(other.begin(), other.end()) { }
		ObjectId(const vector<uint8_t> &other) : vector<uint8_t>(other.begin(), other.end()) { }
	};
	
	typedef enum {
		ObjectNotCached = 0,
		ObjectCached = 1,
		ObjectPartiallyCached = 2
	} CacheAvailability;
	
	struct AppendingToCompletedObjectException : public std::logic_error {
		AppendingToCompletedObjectException()
		: std::logic_error("AppendingToCompletedObjectException") {}
	};
	
	virtual ~Cache() {} 
	
	// Check for existance of an object. This should be very fast 
	// since it might be called multiple times for the same ID 
	// quite often. Avoid OS calls if possible. 
	virtual bool hasObject(const ObjectId &obj_id) = 0; 
	virtual CacheAvailability objectIsAvailable(const ObjectId &obj_id) = 0;
	
	// Read an object from the cache. This should fail if the object has
	// become corrupt, 
	// if the contents has been tampered with by a user, or if the file 
	// has been deleted. 
	virtual bool readObject(const ObjectId &obj_id, vector<uint8_t> &result)=0; 
	
	// Write an object to the cache. If object already exists,
	// it's appended to. If completesInsertion is set, object is marked as
	// complete and cached. If the object is already marked as complete
	// and writeObject is called on it, AppendingToCompletedObjectException
	// is thrown.
	virtual bool writeObject(const ObjectId &obj_id, 
	                         const vector<uint8_t>	 &value,
													 bool completesInsertion = true) = 0; 
	
	// Erase an object from the cache 
	virtual bool eraseObject(const ObjectId &obj_id) = 0; 
	
	// Set a maximum number of bytes that the cache may occupy. 
	// If the cache grows larger than this, old objects must be pruned. 
	virtual void setMaxSize(uint64_t max_size) = 0; 
	
	// Return the current size (in bytes) of the cache 
	virtual uint64_t getCurrentSize() = 0; 
}; 

// Creates an instance of the cache. 
// The encryption key should be used to encrypt all files in the cache.	 
// If the encryption key is wrong, it should behave identically to as if 
// the cache was empty. Use any suitable encryption algorithm. Don't think	
// too much about cryptographic weaknesses, that is not the focus of this 
// test. 
Cache *createCache(const string &path, const vector<uint8_t> &encryption_key); 
 

#endif