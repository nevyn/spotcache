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

#include <sqlite3.h>

class Sqlite {
public:
	class Error : public std::runtime_error {
	public:
		Error(string err) : std::runtime_error(err) {}
		
		int errorCode;
	};
	
	class Statement {
	public:
		Statement(Sqlite &parent);
		~Statement();
		void prepare(const string &statement);
		// Bind values to parameters. i is the index of the parameter.
		void bind(int i, const void*); // blob
		void bind(int i, string); // text
		void bind(int i, int); // int
		
		void reset();
		void step();
	protected:
		Statement(Statement &other) {}; // fix later
		sqlite3_stmt *stmt;
		Sqlite &parent;
	};
	
	
	
	Sqlite(const string &path);
	~Sqlite();
protected:
	sqlite3 *db;
	
	void check(int err);
};