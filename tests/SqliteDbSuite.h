/*
 *  SqliteSuite.h
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-06-27.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */

#include <cxxtest/TestSuite.h>
#include "Sqlite.h"

class CreateDbSuite : public CxxTest::TestSuite {
public:
	Sqlite *db;
	virtual void setUp() {
		db = new Sqlite("test.sqlite");
	}
	virtual void tearDown() {
		delete db;
#ifdef WIN32
		system("del test.sqlite");
#else
		system("rm -f /test.sqlite");
#endif
	}
	
	void testCreateTable( void )
	{
		db->run("create table foo(i int);");
		TS_ASSERT_THROWS_NOTHING(db->run("select * from foo;"));
	}
	
};
