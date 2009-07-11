#ifndef SQLITE__CLASS__H
#define SQLITE__CLASS__H
/*
 *  Sqlite.h
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-06-04.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <stdexcept>
#include <memory>
#include "pstdint.h"
#include <sqlite3.h>

class Sqlite {
public:
	class Error : public std::runtime_error {
	public:
		Error(string err) : std::runtime_error(err) {}
		
		int32_t errorCode;
	};
	
	class Statement {
		friend class Sqlite;
	public:
		typedef std::auto_ptr<Statement> Ptr;
		
		~Statement();
		
		// Bind values to parameters. i is the index of the parameter,
		// and is 1-based
		void bind(int32_t i, const vector<uint8_t>&) throw(Error); // blob
		void bind(int32_t i, const string&) throw(Error); // text
		void bind(int32_t i, int32_t) throw(Error); // int		
		void bind64(int32_t i, int64_t) throw(Error); // int64
		// Reserves n bytes of zeroes
		void bindZeroBlob(int32_t i, int32_t n)  throw(Error); 
		
		// Run the query. For a query that does not return a value
		// or if you don't want the value, run once.
		// Otherwise, run until it returns SQLITE_DONE and between
		// each step, use the column methods to fetch the row data
		int32_t step(int32_t retryCount = 10);
		
		// Check if the last step() resulted in a row being returned.
		// Only if this returns true may you use the column methods.
		bool hasData();
		
		// byval accessors. i is column id and is 0-based
		int32_t intColumn(int32_t i);
		int64_t int64Column(int32_t i);
		double doubleColumn(int32_t i);
		string textColumn(int32_t i);
		vector<uint8_t> blobColumn(int32_t i);
		// byref accessors
		void column(int32_t i, vector<uint8_t> &v);
		
		// Length in bytes of the data at column i
		int32_t byteLength(int32_t i);
		
		
		// If you want to reuse the statement after running it,
		// use reset() before binding new values.
		void reset() throw(Error);
		
		// Create a resetter to auto-reset when a scope in which
		// you use bind() ends
		class Resetter {
		public:
			Resetter(Statement &stmt_) : stmt(stmt_) {}
			~Resetter() { stmt.reset(); }
		protected:
			Statement &stmt;
		};
	protected:
		Statement(Sqlite &parent);
		// Prepares the first statement in 'statement' and returns
		// the rest of the string.
		string prepare(const string &statement);
	
	private:
		sqlite3_stmt *stmt;
		Sqlite &parent;
		int32_t lastStepStatus;
	};
	
	class Blob {
		friend class Sqlite;
	public:
		~Blob();
		typedef std::auto_ptr<Blob> Ptr;
		
		int32_t size();
		// If bytesToRead is not given, size() bytes are read.
		vector<uint8_t> read(int32_t bytesToRead=-1, int32_t offset=0) throw(Error);
		void read(vector<uint8_t>::iterator destination, 
							int32_t bytesToRead=-1,
							int32_t offset = 0) throw(Error);
		void write(const vector<uint8_t> &source, int32_t offset = 0) throw(Error);
		void write(const vector<uint8_t>::const_iterator from,
							 const vector<uint8_t>::const_iterator to,
							 int32_t destOffset = 0) throw(Error);
	protected:
		Blob(Sqlite &parent,
							const string &table, const string &columnName, int64_t rowid);
	private:
		Sqlite &parent;
		sqlite3_blob *blob;
	};
	
	// Prepares the statement in the string 'statement'. If there are
	// several statements in the string, all but the first are ignored.
	Statement::Ptr prepareFirst(const string &statement);
	
	// Open up a Blob for incremental read and write
	Blob::Ptr blob(const string &table, const string &columnName, int64_t rowid);

	// Runs the first statement of a string, ignoring the return value.
	// Will throw with errorCode == SQLITE_BUSY if busy
	void run(const string &statement) throw(Error);
	
	Sqlite(const string &path);
	~Sqlite();
protected:
	sqlite3 *db;
	
	void check(int32_t err) throw(Error);
};
#endif