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
		// Bind values to parameters. i is the index of the parameter.
		void bind(int i, const void*); // blob
		void bind(int i, string); // text
		void bind(int i, int); // int
		
		void reset();
		int step();
		int stepUntilNotBusy(int tryCount = 10);
	protected:
		Statement(Sqlite &parent);
		// Prepares the first statement in 'statement' and returns
		// the rest of the string.
		string prepare(const string &statement);

		sqlite3_stmt *stmt;
		Sqlite &parent;
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