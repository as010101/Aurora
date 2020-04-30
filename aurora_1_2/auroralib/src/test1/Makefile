# Makefile for the Multisim Version
#
# Change the CFLAGS to -native -fast -DCPP for optimal code,
# runs well with qjobs
#

# A default value for when we're building this subsystem independently...
ifndef CFLAGS_SET_TO_USE
   CFLAGS_SET_TO_USE=USE_NORMAL_DEV_CFLAGS
endif

include ../../../makedefs_locations.mk
include ../../../makedefs_cflags.mk
include ../../../makedefs_util.mk

INCLUDE_FLAGS= \
	-I. \
	-I$(AURORA_INCLUDE_DIR)

AURORA_API_LIB=$(AURORA_LIB_DIR)/libaurora_cxx.a
AURORA_API_BIN_DIR=$(AURORA_API_BASE_DIR)/bin

TEST1_APP=$(AURORA_API_BIN_DIR)/test1

TEST1_OBJECTS= \
	main.o

$(TEST1_APP): \
	$(AURORA_API_LIB) \
	$(TEST1_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(TEST1_OBJECTS) \
	$(AURORA_API_LIB) \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)


%.o: %.C
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $<

clean: 
	rm -f  *.o core *~
	rm -f $(TEST1_APP)
