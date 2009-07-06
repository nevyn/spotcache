#ifndef SPOT_CACHE__H
#define SPOT_CACHE__H

/// Joachim Bengtsson
/// joachimb@gmail.com
/// 2009-06-03

#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
using std::vector;
using std::string;


class Cache { 
	friend class Partial;
public: 
	// --- Types ---

	class ObjectId : public vector<uint8_t> {
	public:
		ObjectId(const string &other)
			: vector<uint8_t>(other.begin(), other.end()) { }
		ObjectId(const char *other)
		  : vector<uint8_t>(other, other+strlen(other)) { }
		ObjectId(const vector<uint8_t> &other)
			: vector<uint8_t>(other.begin(), other.end()) { }
		ObjectId(size_type __n)
			: vector<uint8_t>(__n) { }
	};
	
	typedef enum {
		ObjectNotCached = 0,
		ObjectCached = 1,
		ObjectPartiallyCached = 2
	} CacheAvailability;	
	
	class Partial {
	public:
		typedef std::auto_ptr<Partial> Ptr;

		/// A Partial saves a position indicator similar to how fread/fwrite works.
		virtual uint64_t positionIndicator() = 0;
		/// Like fseek()
		virtual uint64_t seek(uint64_t, int whence) = 0;
		
		/// Write at the position indicator and increment it
		virtual void append(const vector<uint8_t> &value) = 0;
		/// Write at offset
		virtual void writeAt(uint64_t offset, const vector<uint8_t> &value) = 0;
		
		/// Resizes the cache object. If smaller than current, truncates. If bigger,
		/// fills the new space with zeroes.
		virtual void resize(uint64_t new_size) = 0;
		
		virtual ~Partial() = 0;
	};
		
	virtual ~Cache() {} 
	
	// --- Methods ---
	
	/// Check for existance of an object. This should be very fast 
	/// since it might be called multiple times for the same ID 
	/// quite often. Avoid OS calls if possible. 
	virtual bool hasObject(const ObjectId &obj_id) = 0; 
	virtual CacheAvailability objectIsAvailable(const ObjectId &obj_id) = 0;
	
	/// Read an object from the cache. This should fail if the object has
	/// become corrupt, 
	/// if the contents has been tampered with by a user, or if the file 
	/// has been deleted. 
	virtual bool readObject(const ObjectId &obj_id, vector<uint8_t> &result)=0; 
	
	/// Write a complete object to the cache.
	/// @returns true on success
	/// @returns false if the object is too big to fit in cache
	virtual bool writeObject(const ObjectId &obj_id, 
	                         const vector<uint8_t>	 &value) = 0;
	
	/// Begin a partial object transaction. While the Partial object exists,
	/// the given object will be unavailable for reading and writing outside of
	/// its accessors.
	/// @arg obj_id If this object already exists, it's turned into a
	///							partial without otherwise being overwritten
	/// @arg bytesToReserve How much space to reserve
	/// @returns a null-pointed autoptr for the same reasons writeObject might,
	///					 fail, and also if there is already a Partial for this object.
	/// @returns autoptr to Partial object on success.
	virtual Partial::Ptr partial(const ObjectId &obj_id, uint64_t bytesToReserve) = 0;

	// Erase an object from the cache 
	virtual bool eraseObject(const ObjectId &obj_id) = 0; 
	
	/// Set a maximum number of bytes that the cache may occupy. 
	/// If the cache grows larger than this, old objects will be pruned. 
	virtual void setMaxSize(uint64_t max_size) = 0; 
	
	/// Return the current size (in bytes) of the cache 
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