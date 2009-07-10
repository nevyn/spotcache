/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "tests/SqliteDbSuite.h"

static CreateDbSuite suite_CreateDbSuite;

static CxxTest::List Tests_CreateDbSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CreateDbSuite( "tests/SqliteDbSuite.h", 13, "CreateDbSuite", suite_CreateDbSuite, Tests_CreateDbSuite );

static class TestDescription_CreateDbSuite_testCreateTable : public CxxTest::RealTestDescription {
public:
 TestDescription_CreateDbSuite_testCreateTable() : CxxTest::RealTestDescription( Tests_CreateDbSuite, suiteDescription_CreateDbSuite, 24, "testCreateTable" ) {}
 void runTest() { suite_CreateDbSuite.testCreateTable(); }
} testDescription_CreateDbSuite_testCreateTable;

#include "tests/SqliteTableSuite.h"

static CreateTableSuite suite_CreateTableSuite;

static CxxTest::List Tests_CreateTableSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CreateTableSuite( "tests/SqliteTableSuite.h", 11, "CreateTableSuite", suite_CreateTableSuite, Tests_CreateTableSuite );

static class TestDescription_CreateTableSuite_testInsert : public CxxTest::RealTestDescription {
public:
 TestDescription_CreateTableSuite_testInsert() : CxxTest::RealTestDescription( Tests_CreateTableSuite, suiteDescription_CreateTableSuite, 24, "testInsert" ) {}
 void runTest() { suite_CreateTableSuite.testInsert(); }
} testDescription_CreateTableSuite_testInsert;

static class TestDescription_CreateTableSuite_testBlob : public CxxTest::RealTestDescription {
public:
 TestDescription_CreateTableSuite_testBlob() : CxxTest::RealTestDescription( Tests_CreateTableSuite, suiteDescription_CreateTableSuite, 60, "testBlob" ) {}
 void runTest() { suite_CreateTableSuite.testBlob(); }
} testDescription_CreateTableSuite_testBlob;

#include "tests/SqliteCacheSuite.h"

static SqliteCacheSuite *suite_SqliteCacheSuite = 0;

static CxxTest::List Tests_SqliteCacheSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<SqliteCacheSuite> suiteDescription_SqliteCacheSuite( "tests/SqliteCacheSuite.h", 27, "SqliteCacheSuite", Tests_SqliteCacheSuite, suite_SqliteCacheSuite, 41, 42 );

static class TestDescription_SqliteCacheSuite_testAvailabilityOfNonExistingObject : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testAvailabilityOfNonExistingObject() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 47, "testAvailabilityOfNonExistingObject" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testAvailabilityOfNonExistingObject(); }
} testDescription_SqliteCacheSuite_testAvailabilityOfNonExistingObject;

static class TestDescription_SqliteCacheSuite_testHasObjectOfNonExistingObject : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testHasObjectOfNonExistingObject() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 52, "testHasObjectOfNonExistingObject" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testHasObjectOfNonExistingObject(); }
} testDescription_SqliteCacheSuite_testHasObjectOfNonExistingObject;

static class TestDescription_SqliteCacheSuite_testReadingNonExistingObject : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testReadingNonExistingObject() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 56, "testReadingNonExistingObject" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testReadingNonExistingObject(); }
} testDescription_SqliteCacheSuite_testReadingNonExistingObject;

static class TestDescription_SqliteCacheSuite_testWritePartial : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testWritePartial() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 64, "testWritePartial" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testWritePartial(); }
} testDescription_SqliteCacheSuite_testWritePartial;

static class TestDescription_SqliteCacheSuite_testPartialAvailability : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testPartialAvailability() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 69, "testPartialAvailability" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testPartialAvailability(); }
} testDescription_SqliteCacheSuite_testPartialAvailability;

static class TestDescription_SqliteCacheSuite_testHasObjectOfPartialObject : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testHasObjectOfPartialObject() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 74, "testHasObjectOfPartialObject" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testHasObjectOfPartialObject(); }
} testDescription_SqliteCacheSuite_testHasObjectOfPartialObject;

static class TestDescription_SqliteCacheSuite_testReadingPartial : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testReadingPartial() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 78, "testReadingPartial" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testReadingPartial(); }
} testDescription_SqliteCacheSuite_testReadingPartial;

static class TestDescription_SqliteCacheSuite_testTwoPartialsOneObject : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testTwoPartialsOneObject() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 93, "testTwoPartialsOneObject" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testTwoPartialsOneObject(); }
} testDescription_SqliteCacheSuite_testTwoPartialsOneObject;

static class TestDescription_SqliteCacheSuite_testFinalizingPartial : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testFinalizingPartial() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 98, "testFinalizingPartial" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testFinalizingPartial(); }
} testDescription_SqliteCacheSuite_testFinalizingPartial;

static class TestDescription_SqliteCacheSuite_testCompleteAvailability : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testCompleteAvailability() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 113, "testCompleteAvailability" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testCompleteAvailability(); }
} testDescription_SqliteCacheSuite_testCompleteAvailability;

static class TestDescription_SqliteCacheSuite_testHasObjectOfCompleteObject : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testHasObjectOfCompleteObject() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 119, "testHasObjectOfCompleteObject" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testHasObjectOfCompleteObject(); }
} testDescription_SqliteCacheSuite_testHasObjectOfCompleteObject;

static class TestDescription_SqliteCacheSuite_testTurningExistingObjectIntoPartial : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testTurningExistingObjectIntoPartial() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 123, "testTurningExistingObjectIntoPartial" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testTurningExistingObjectIntoPartial(); }
} testDescription_SqliteCacheSuite_testTurningExistingObjectIntoPartial;

static class TestDescription_SqliteCacheSuite_testGettingSize : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testGettingSize() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 136, "testGettingSize" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testGettingSize(); }
} testDescription_SqliteCacheSuite_testGettingSize;

static class TestDescription_SqliteCacheSuite_testOversteppingCacheSize : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testOversteppingCacheSize() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 141, "testOversteppingCacheSize" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testOversteppingCacheSize(); }
} testDescription_SqliteCacheSuite_testOversteppingCacheSize;

static class TestDescription_SqliteCacheSuite_testCorruptingStore : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testCorruptingStore() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 172, "testCorruptingStore" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testCorruptingStore(); }
} testDescription_SqliteCacheSuite_testCorruptingStore;

static class TestDescription_SqliteCacheSuite_testPersistsMaxSize : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testPersistsMaxSize() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 195, "testPersistsMaxSize" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testPersistsMaxSize(); }
} testDescription_SqliteCacheSuite_testPersistsMaxSize;

#include <cxxtest/Root.cpp>
