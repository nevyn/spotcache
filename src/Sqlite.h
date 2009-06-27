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

#include <sqlite3.h>

class Sqlite {
public:
	class Error : public std::runtime_error {
	public:
		Error(string err) : std::runtime_error(err) {}
		
		int errorCode;
	};
	
	class Statement {
		friend class Sqlite;
	public:
		typedef std::auto_ptr<Statement> Ptr;
		
		~Statement();
		
		// Bind values to parameters. i is the index of the parameter,
		// and is 1-based
		void bind(int i, const vector<uint8_t>&); // blob
		void bind(int i, const string&); // text
		void bind(int i, int); // int
		
		// Run the query. For a query that does not return a value
		// or if you don't want the value, run once.
		// Otherwise, run until it returns SQLITE_DONE and between
		// each step, use the column methods to fetch the row data
		int step(int retryCount = 10);
		
		// Check if the last step() resulted in a row being returned.
		// Only if this returns true may you use the column methods.
		bool hasData();
		
		// byval accessors. i is column id and is 0-based
		int intColumn(int i);
		double doubleColumn(int i);
		string textColumn(int i);
		vector<uint8_t> blobColumn(int i);
		// byref accessors
		void column(int i, vector<uint8_t> &v);
		
		// Length in bytes of the data at column i
		int byteLength(int i);
		
		
		// If you want to reuse the statement after running it,
		// use reset() before binding new values.
		void reset();
		
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

		sqlite3_stmt *stmt;
		Sqlite &parent;
		int lastStepStatus;
	};
	
	// Prepares the statement in the string 'statement'. If there are
	// several statements in the string, all but the first are ignored.
	Statement::Ptr prepareFirst(const string &statement);

	// Runs the first statement of a string, ignoring the return value.
	void run(const string &statement);
	
	Sqlite(const string &path);
	~Sqlite();
protected:
	sqlite3 *db;
	
	void check(int err);
};