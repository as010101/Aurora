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
	$(OBJ_BUFFERCACHE) \
	install_obj \
	testBufferCacheControlBlock \
	testBufferCache \
	testAvailablePageSet

install_obj:
	cp $(OBJ_BUFFERCACHE) $(SM_TARGETBASE)/obj

clean:
	rm -f *.o
	rm -f testBufferCacheControlBlock
	rm -f testBufferCache
	rm -f testAvailablePageSet

testBufferCache: \
	$(OBJ_BUFFERCACHE) \
	testBufferCache.o
	$(CC) $(CFLAGS) -o $@ \
	testBufferCache.o \
	$(SM_TARGETBASE)/obj/*.o \
	$(UTIL_LIB_DIR)/util.a \
	-l pthread  \
	$(XERCES_LIB_LINKAGE) \
	$(GNU_MP_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

testBufferCacheControlBlock: \
	$(OBJ_BUFFERCACHE) \
	testBufferCacheControlBlock.o
	$(CC) $(CFLAGS) -o $@ \
	testBufferCacheControlBlock.o \
	$(SM_TARGETBASE)/obj/*.o \
	$(UTIL_LIB_DIR)/util.a \
	-l pthread \
	$(XERCES_LIB_LINKAGE) \
	$(GNU_MP_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

testAvailablePageSet: \
	$(OBJ_BUFFERCACHE) \
	testAvailablePageSet.o
	$(CC) $(CFLAGS) -o $@ \
	testAvailablePageSet.o \
	$(SM_TARGETBASE)/obj/*.o \
	$(UTIL_LIB_DIR)/util.a \
	-l pthread  \
	$(XERCES_LIB_LINKAGE) \
	$(GNU_MP_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

# makedepend checks file dependencies and makes sure that everything
# that needs to be compiled is compiled. It's not an entirely speedy
# process, but you should run it after adding header files and/or
# making significant changes to your code base.
# Commented out because these cause such pain for people. -cjc
#dependencies:
#	makedepend $(INCLUDE_FLAGS) $(CPLUSPLUS_STDLIB_MAKEDEP_INCLUDE_FLAGS) *.C
