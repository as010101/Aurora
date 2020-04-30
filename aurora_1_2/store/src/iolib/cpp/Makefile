ifndef CFLAGS_SET_TO_USE
   CFLAGS_SET_TO_USE=USE_NORMAL_DEV_CFLAGS
endif

include ../../../../makedefs_locations.mk
include ../../../../makedefs_cflags.mk
include ../../../../makedefs_util.mk
include ../../sm_makedefs.mk

INCLUDE_FLAGS= \
	-I$(UTIL_INCLUDE_DIR) \
	-I$(SM_PROJBASE)/src \
	$(XERCES_INCLUDE_CFLAG)

CFLAGS += $(INCLUDE_FLAGS)

all: \
	$(OBJ_IOLIB) \
	testPageFile \
	testPageDir \
	testAsynchPageDir \
	benchmarkAsynchPageDir \
	install

testAsynchPageDir: testAsynchPageDir.o AsynchPageDir.o PageDir.o PageFile.o PageAddr.o \
	ApdMasterRunner.o RwOpRunner.o PdGrowthRunner.o IOCB.o
	$(CC) $(CFLAGS) -o $@ testAsynchPageDir.o \
	AsynchPageDir.o \
	PageDir.o \
	PageFile.o \
	PageAddr.o \
	ApdMasterRunner.o \
	RwOpRunner.o \
	PdGrowthRunner.o \
	IOCB.o \
	$(UTIL_LIB_DIR)/util.a \
	-lpthread \
	$(XERCES_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

benchmarkAsynchPageDir: benchmarkAsynchPageDir.o AsynchPageDir.o PageDir.o PageFile.o PageAddr.o \
	ApdMasterRunner.o RwOpRunner.o PdGrowthRunner.o IOCB.o
	$(CC) $(CFLAGS) -o $@ benchmarkAsynchPageDir.o \
	AsynchPageDir.o \
	PageDir.o \
	PageFile.o \
	PageAddr.o \
	ApdMasterRunner.o \
	RwOpRunner.o \
	PdGrowthRunner.o \
	IOCB.o \
	$(UTIL_LIB_DIR)/util.a \
	-lpthread \
	$(XERCES_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

testPageDir: testPageDir.o PageDir.o
	$(CC) $(CFLAGS) -o $@ testPageDir.o \
	PageDir.o \
	PageFile.o \
	PageAddr.o \
	$(UTIL_LIB_DIR)/util.a \
	$(XERCES_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

testPageFile: testPageFile.o PageFile.o 
	$(CC) $(CFLAGS) -o $@ testPageFile.o \
	PageFile.o \
	$(UTIL_LIB_DIR)/util.a \
	$(XERCES_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)


install:
	cp $(OBJ_IOLIB) $(SM_TARGETBASE)/obj

clean:
	rm -f *.o
	rm -f testPageFile
	rm -f testPageDir
	rm -f testAsynchPageDir
	rm -f benchmarkAsynchPageDir

# makedepend checks file dependencies and makes sure that everything
# that needs to be compiled is compiled. It's not an entirely speedy
# process, but you should run it after adding header files and/or
# making significant changes to your code base.
# Commented out because these cause such pain for people. -cjc
#dependencies:
#	makedepend $(INCLUDE_FLAGS) $(CPLUSPLUS_STDLIB_MAKEDEP_INCLUDE_FLAGS) *.C
