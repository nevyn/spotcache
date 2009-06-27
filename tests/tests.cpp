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
CxxTest::StaticSuiteDescription suiteDescription_CreateTableSuite( "tests/SqliteTableSuite.h", 10, "CreateTableSuite", suite_CreateTableSuite, Tests_CreateTableSuite );

static class TestDescription_CreateTableSuite_testInsert : public CxxTest::RealTestDescription {
public:
 TestDescription_CreateTableSuite_testInsert() : CxxTest::RealTestDescription( Tests_CreateTableSuite, suiteDescription_CreateTableSuite, 22, "testInsert" ) {}
 void runTest() { suite_CreateTableSuite.testInsert(); }
} testDescription_CreateTableSuite_testInsert;

#include "tests/SqliteCacheSuite.h"

static SqliteCacheSuite *suite_SqliteCacheSuite = 0;

static CxxTest::List Tests_SqliteCacheSuite = { 0, 0 };
CxxTest::DynamicSuiteDescription<SqliteCacheSuite> suiteDescription_SqliteCacheSuite( "tests/SqliteCacheSuite.h", 27, "SqliteCacheSuite", Tests_SqliteCacheSuite, suite_SqliteCacheSuite, 43, 44 );

static class TestDescription_SqliteCacheSuite_testAvailabilityOfNonExistingObject : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testAvailabilityOfNonExistingObject() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 49, "testAvailabilityOfNonExistingObject" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testAvailabilityOfNonExistingObject(); }
} testDescription_SqliteCacheSuite_testAvailabilityOfNonExistingObject;

static class TestDescription_SqliteCacheSuite_testReadingNonExistingObject : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testReadingNonExistingObject() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 54, "testReadingNonExistingObject" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testReadingNonExistingObject(); }
} testDescription_SqliteCacheSuite_testReadingNonExistingObject;

static class TestDescription_SqliteCacheSuite_testWritePartial : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testWritePartial() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 60, "testWritePartial" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testWritePartial(); }
} testDescription_SqliteCacheSuite_testWritePartial;

static class TestDescription_SqliteCacheSuite_testPartialAvailability : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testPartialAvailability() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 65, "testPartialAvailability" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testPartialAvailability(); }
} testDescription_SqliteCacheSuite_testPartialAvailability;

static class TestDescription_SqliteCacheSuite_testReadingPartial : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testReadingPartial() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 70, "testReadingPartial" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testReadingPartial(); }
} testDescription_SqliteCacheSuite_testReadingPartial;

static class TestDescription_SqliteCacheSuite_testFinalizingPartial : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testFinalizingPartial() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 77, "testFinalizingPartial" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testFinalizingPartial(); }
} testDescription_SqliteCacheSuite_testFinalizingPartial;

static class TestDescription_SqliteCacheSuite_testFinalizingAlreadyFinalized : public CxxTest::RealTestDescription {
public:
 TestDescription_SqliteCacheSuite_testFinalizingAlreadyFinalized() : CxxTest::RealTestDescription( Tests_SqliteCacheSuite, suiteDescription_SqliteCacheSuite, 92, "testFinalizingAlreadyFinalized" ) {}
 void runTest() { if ( suite_SqliteCacheSuite ) suite_SqliteCacheSuite->testFinalizingAlreadyFinalized(); }
} testDescription_SqliteCacheSuite_testFinalizingAlreadyFinalized;

#include <cxxtest/Root.cpp>
