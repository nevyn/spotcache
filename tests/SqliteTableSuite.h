/*
 *  SqliteTableSuite.h
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-06-27.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */

class CreateTableSuite : public CxxTest::TestSuite {
public:
	Sqlite *db;
	virtual void setUp() {
		db = new Sqlite("/test.sqlite");
		db->run("create table foo(i int);");
	}
	virtual void tearDown() {
		delete db;
		system("rm -f /test.sqlite");
	}
	
	void testInsert( void )
	{
		static int32_t insertedValue = 42;
		Sqlite::Statement::Ptr insert = db->prepareFirst("insert into foo values (?);");
		insert->bind(1, insertedValue);
		TS_ASSERT_THROWS_NOTHING(insert->step());
		insert->reset();
		
		Sqlite::Statement::Ptr confirm = db->prepareFirst("select * from foo where i = 42;");
		int32_t status;
		TS_ASSERT_THROWS_NOTHING(status = confirm->step());
		TS_ASSERT_EQUALS(status, SQLITE_ROW);
		int32_t fetchedValue = confirm->intColumn(0);
		TS_ASSERT_EQUALS(fetchedValue, insertedValue);
		
		TS_ASSERT_THROWS_NOTHING(status = confirm->step());
		TS_ASSERT_EQUALS(status, SQLITE_DONE);
	}
	
};
