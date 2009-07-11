/*
 *  SqliteTableSuite.h
 *  SpotCache
 *
 *  Created by Joachim Bengtsson on 2009-06-27.
 *  Copyright 2009 Third Cog Software. All rights reserved.
 *
 */
#include <algorithm>

class CreateTableSuite : public CxxTest::TestSuite {
public:
	Sqlite *db;
	virtual void setUp() {
		db = new Sqlite("test.sqlite");
		db->run("create table if not exists foo(i int);");
		db->run("create table if not exists bar(bytes blob);");
	}
	virtual void tearDown() {
		delete db;
#ifdef WIN32
		system("del test.sqlite");
#else
		system("rm -f test.sqlite");
#endif
	}
	
	void testInsert( void )
	{
		// Insert the answer to life, the universe and everything into our dummy table
		static int32_t insertedValue = 42;
		
		// Make sure prepare throws if we feed it bogus data
		TS_ASSERT_THROWS(db->prepareFirst("niu32tnekjd not some sql I can tell you that#%/)"), Sqlite::Error);
		
		// Sane statement shouldn't throw, though.
		Sqlite::Statement::Ptr insert = db->prepareFirst("insert into foo values (?);");
		
		// Bind the value we want to insert. This can't fail, as it doesn't do anything
		insert->bind(1, insertedValue);
		
		// step() will perform the actual insertion. This can, but shouldn't fail.
		TS_ASSERT_THROWS_NOTHING(insert->step());
		
		// Read the value back and make sure it's correct.
		Sqlite::Statement::Ptr confirm = db->prepareFirst("select * from foo where i = 42;");
		int32_t status;
		// Stepping now SHOULD return a row, and not "Done" (as that would mean there's no data,
		// and the statement finished without returning data)
		TS_ASSERT_THROWS_NOTHING(status = confirm->step());
		TS_ASSERT_EQUALS(status, SQLITE_ROW);
		
		// Fetch the value; it'll be in column 0 (the only column in this table).
		int32_t fetchedValue = confirm->intColumn(0);
		
		// It should be the same as what we inserted before
		TS_ASSERT_EQUALS(fetchedValue, insertedValue);
		
		// Stepping now should return DONE, as there should be no more data in that table.
		TS_ASSERT_THROWS_NOTHING(status = confirm->step());
		TS_ASSERT_EQUALS(status, SQLITE_DONE);
	}
	
	void testBlob()
	{
		// Make a new entry in 'bar' which is just a zero'd blob, 8 bytes long.
		Sqlite::Statement::Ptr insert = db->prepareFirst("insert into bar values (?);");
		insert->bindZeroBlob(1, 8);
		TS_ASSERT_THROWS_NOTHING(insert->step());
		
		// Fetch the row id so we have a reference to make a Blob object of
		Sqlite::Statement::Ptr getRowid = db->prepareFirst("select rowid from bar;");
		getRowid->step();
		int64_t rowid = getRowid->int64Column(0);
		
		// Here's our blob object
		Sqlite::Blob::Ptr blob = db->blob("bar", "bytes", rowid);
		
		// It should be just eight zero'd bytes.
		vector<uint8_t> zeroes(8);
		vector<uint8_t> sofar = blob->read();
		TS_ASSERT_SAME_DATA(&(*sofar.begin()), &(*zeroes.begin()), 8);
		
		// Prepare some data we will write, 8 bytes of it.
		vector<uint8_t> towrite(8);
		string onetwothree = "1234567";
		std::copy(onetwothree.begin(), onetwothree.end(), towrite.begin());
		
		// Write the second half, to test the offset parameter
		blob->write(towrite.begin()+4, towrite.end(), 4);
		
		// Make sure we now have zeroes in the first half, and actual data
		// in the second half.
		const char *expecthere = "\0\0\0\0""567";
		sofar = blob->read();
		TS_ASSERT_SAME_DATA(&(*sofar.begin()), expecthere, 8);
		
		// Write the rest.
		vector<uint8_t> beginning(towrite.begin(), towrite.begin()+4);
		blob->write(beginning);
		
		// Make sure we got the right data in there.
		sofar = blob->read();
		TS_ASSERT_SAME_DATA(&(*sofar.begin()), onetwothree.c_str(), 8);
	}
	
};
