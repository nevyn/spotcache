OBJS = src/Cache.o \
       src/Sqlite.o src/Sqlite+Blob.o src/Sqlite+Statement.o \
       src/SqliteCache.o src/SqliteCache+Partial.o
VENDOROBJS = vendor/md5/md5c.o vendor/sqlite/sqlite3.o
TESTOBJS = $(OBJS) $(VENDOROBJS) tests/tests.o
PERFOBJS = $(OBJS) $(VENDOROBJS) tests/performancetest.o
CXXFLAGS = -Ivendor -Ivendor/cxxtest -Ivendor/md5 -Ivendor/sqlite -Isrc -I.

test: $(TESTOBJS) Makefile
	g++ -o test $(TESTOBJS)

perftest: $(PERFOBJS) Makefile
	g++ -o perftest $(PERFOBJS)

.PHONY: clean

clean:
	rm -f src/*.o tests/*.o vendor/md5/*.o vendor/sqlite/*.o test perftest
